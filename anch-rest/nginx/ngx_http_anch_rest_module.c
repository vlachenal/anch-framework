/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++23 standard
  Copyright (C) 2026 Vincent Lachenal

  This file is part of ANCH Framework.

  ANCH Framework is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  ANCH Framework is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with ANCH Framework.  If not, see <https://www.gnu.org/licenses/>.
*/

/*
 * AnCH REST nginx module.
 *
 * Directives:
 *   anch_rest_app <library.so> [configuration];   serve location with an AnCH REST application
 *   anch_rest_thread_pool <name>|off;             thread pool for handlers (default: "default")
 *   anch_rest_buffers <number> <size>;            response buffers of a request (default: 4 16k)
 *
 * Applications are loaded with dlopen when configuration is read and created in each worker process.
 *
 * Request and response bodies are streamed.
 *
 * In a thread pool (nginx built with --with-threads), the handler is started as soon as request headers have been
 * read:
 * - the request body is not buffered (request_body_no_buffering): the handler reads it while nginx receives it, at
 *   most client_body_buffer_size bytes are waiting for the handler ;
 * - the response body is written in at most <number> buffers of <size> bytes: the handler waits while they are
 *   being sent.
 * The handler thread never uses nginx structures: data are exchanged with the event loop through a shared request
 * context (mutex and condition) and a notification pipe per worker process. A slow client keeps a thread busy while
 * it sends its request body or receives the end of the response (client_body_timeout and send_timeout apply).
 *
 * Without thread pool (anch_rest_thread_pool off, or nginx built without threads), the handler is executed in the
 * event loop (blocking): the request body is buffered by nginx (memory then temporary file) and read in blocks, the
 * response body is given to nginx as it is written (without flow control).
 */
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#if (NGX_THREADS)
#include <ngx_channel.h>
#endif

#include <dlfcn.h>
#include <stdlib.h>
#include <unistd.h>

#include "rest/anch_rest_nginx.h"

/* Types + */
typedef struct {
  ngx_str_t                      path;
  ngx_str_t                      config;
  void                          *handle;
  const anch_rest_application_t *ops;
  void                          *instance;   /* created in worker process */
} ngx_http_anch_rest_app_t;

typedef struct {
  ngx_array_t apps;  /* ngx_http_anch_rest_app_t* */
} ngx_http_anch_rest_main_conf_t;

typedef struct {
  ngx_http_anch_rest_app_t *app;
  ngx_str_t                 pool_name;
  ngx_flag_t                pool_off;
  ngx_bufs_t                bufs;
#if (NGX_THREADS)
  ngx_thread_pool_t        *pool;
#endif
} ngx_http_anch_rest_loc_conf_t;

typedef struct ngx_http_anch_rest_ctx_s ngx_http_anch_rest_ctx_t;

#if (NGX_THREADS)
/* Request body segment: data of an nginx buffer given to the handler */
typedef struct ngx_http_anch_rest_seg_s ngx_http_anch_rest_seg_t;

struct ngx_http_anch_rest_seg_s {
  ngx_http_anch_rest_seg_t *next;
  ngx_buf_t                *buf;   /* nginx buffer, consumed when segment has been released (event loop) */
  u_char                   *pos;   /* data not read yet (handler) */
  u_char                   *last;
};

/* Response buffer */
typedef struct {
  ngx_buf_t   *buf;
  ngx_chain_t  chain;    /* link given to filters (they copy links) */
  size_t       len;      /* filled length */
  ngx_uint_t   sending;  /* given to nginx (event loop) */
} ngx_http_anch_rest_out_t;
#endif

struct ngx_http_anch_rest_ctx_s {
  ngx_http_request_t            *request;
  ngx_http_anch_rest_app_t      *app;
  anch_rest_request_t            in;
  anch_rest_response_writer_t    writer;
  int                            result;        /* handle result */

  /* Event loop state */
  ngx_int_t                      header_rc;     /* ngx_http_send_header result */
  unsigned                       header_sent:1;
  unsigned                       header_only:1;
  unsigned                       failed:1;      /* response can not be sent anymore */
  unsigned                       reading:1;     /* request body is being read (thread pool) */
  ngx_chain_t                   *in_cl;         /* buffered body reader (no thread pool) */
  off_t                          in_offset;

#if (NGX_THREADS)
  ngx_thread_mutex_t             mutex;
  ngx_thread_cond_t              cond;
  ngx_log_t                     *log;           /* cycle log: used by handler thread */
  ngx_uint_t                     threaded;      /* mutex and condition have been created */

  /* Shared state (mutex) + */
  ngx_uint_t                     cancelled;     /* response can not be sent anymore */
  /* request body */
  ngx_http_anch_rest_seg_t      *in_head;       /* first segment to read (handler) */
  ngx_http_anch_rest_seg_t      *in_tail;       /* last segment (event loop) */
  ngx_http_anch_rest_seg_t      *in_release;    /* first segment not released (event loop) */
  ngx_uint_t                     in_eof;
  ngx_uint_t                     in_error;
  ngx_int_t                      in_status;     /* HTTP status of reading error (413, 408, ...) */
  /* response buffers */
  ngx_http_anch_rest_out_t      *out;
  ngx_uint_t                     out_count;     /* allocated buffers */
  ngx_uint_t                     out_max;
  size_t                         out_size;
  ngx_uint_t                    *out_free;      /* free buffers (stack) */
  ngx_uint_t                     out_free_count;
  ngx_uint_t                    *out_full;      /* full buffers (FIFO) */
  ngx_uint_t                     out_full_count;
  ngx_int_t                      out_current;   /* buffer being filled (handler), -1 when none */
  ngx_uint_t                     out_want;      /* handler waits for a new buffer */
  /* response start */
  ngx_uint_t                     started;       /* start has been called */
  ngx_uint_t                     start_pending;
  ngx_uint_t                     start_status;
  anch_rest_header_t            *start_headers; /* malloc'ed copy */
  size_t                         start_count;
  long long                      start_length;
  /* notification channel (channel mutex) */
  ngx_uint_t                     queued;        /* 0: no, 1: in list, 2: being processed, 3: processed and queued again */
  ngx_http_anch_rest_ctx_t      *ready_next;
  /* Shared state (mutex) - */

  ngx_uint_t                    *sendq;         /* full buffers taken by event loop */
  ngx_uint_t                    *freeq;         /* buffers released by event loop */
#endif
};

#if (NGX_THREADS)
/* Notification channel of worker process: handler threads wake the event loop up */
typedef struct {
  ngx_thread_mutex_t        mutex;
  ngx_fd_t                  fds[2];
  ngx_http_anch_rest_ctx_t *head;
  ngx_uint_t                ready;
} ngx_http_anch_rest_channel_t;

static ngx_http_anch_rest_channel_t ngx_http_anch_rest_channel;
#endif
/* Types - */

/* Declarations + */
static char *ngx_http_anch_rest_app(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static char *ngx_http_anch_rest_thread_pool(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static void *ngx_http_anch_rest_create_main_conf(ngx_conf_t *cf);
static void *ngx_http_anch_rest_create_loc_conf(ngx_conf_t *cf);
static char *ngx_http_anch_rest_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child);
static ngx_int_t ngx_http_anch_rest_init_process(ngx_cycle_t *cycle);
static void ngx_http_anch_rest_exit_process(ngx_cycle_t *cycle);
static ngx_int_t ngx_http_anch_rest_handler(ngx_http_request_t *r);
static void ngx_http_anch_rest_body_handler(ngx_http_request_t *r);
/* Declarations - */

/* Module + */
static ngx_command_t ngx_http_anch_rest_commands[] = {
  {
    ngx_string("anch_rest_app"),
    NGX_HTTP_LOC_CONF | NGX_CONF_TAKE12,
    ngx_http_anch_rest_app,
    NGX_HTTP_LOC_CONF_OFFSET,
    0,
    NULL
  },
  {
    ngx_string("anch_rest_thread_pool"),
    NGX_HTTP_MAIN_CONF | NGX_HTTP_SRV_CONF | NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
    ngx_http_anch_rest_thread_pool,
    NGX_HTTP_LOC_CONF_OFFSET,
    0,
    NULL
  },
  {
    ngx_string("anch_rest_buffers"),
    NGX_HTTP_MAIN_CONF | NGX_HTTP_SRV_CONF | NGX_HTTP_LOC_CONF | NGX_CONF_TAKE2,
    ngx_conf_set_bufs_slot,
    NGX_HTTP_LOC_CONF_OFFSET,
    offsetof(ngx_http_anch_rest_loc_conf_t, bufs),
    NULL
  },
  ngx_null_command
};

static ngx_http_module_t ngx_http_anch_rest_module_ctx = {
  NULL,                                /* preconfiguration */
  NULL,                                /* postconfiguration */
  ngx_http_anch_rest_create_main_conf, /* create main configuration */
  NULL,                                /* init main configuration */
  NULL,                                /* create server configuration */
  NULL,                                /* merge server configuration */
  ngx_http_anch_rest_create_loc_conf,  /* create location configuration */
  ngx_http_anch_rest_merge_loc_conf    /* merge location configuration */
};

ngx_module_t ngx_http_anch_rest_module = {
  NGX_MODULE_V1,
  &ngx_http_anch_rest_module_ctx,  /* module context */
  ngx_http_anch_rest_commands,     /* module directives */
  NGX_HTTP_MODULE,                 /* module type */
  NULL,                            /* init master */
  NULL,                            /* init module */
  ngx_http_anch_rest_init_process, /* init process */
  NULL,                            /* init thread */
  NULL,                            /* exit thread */
  ngx_http_anch_rest_exit_process, /* exit process */
  NULL,                            /* exit master */
  NGX_MODULE_V1_PADDING
};
/* Module - */

/* Configuration + */
static void *
ngx_http_anch_rest_create_main_conf(ngx_conf_t *cf)
{
  ngx_http_anch_rest_main_conf_t *amcf;

  amcf = ngx_pcalloc(cf->pool, sizeof(ngx_http_anch_rest_main_conf_t));
  if (amcf == NULL) {
    return NULL;
  }
  if (ngx_array_init(&amcf->apps, cf->pool, 4, sizeof(ngx_http_anch_rest_app_t *)) != NGX_OK) {
    return NULL;
  }
  return amcf;
}

static void *
ngx_http_anch_rest_create_loc_conf(ngx_conf_t *cf)
{
  ngx_http_anch_rest_loc_conf_t *alcf;

  alcf = ngx_pcalloc(cf->pool, sizeof(ngx_http_anch_rest_loc_conf_t));
  if (alcf == NULL) {
    return NULL;
  }
  /* set by ngx_pcalloc(): alcf->bufs.num = 0 (unset) */
  alcf->app = NULL;
  alcf->pool_off = NGX_CONF_UNSET;
  return alcf;
}

static char *
ngx_http_anch_rest_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child)
{
  ngx_http_anch_rest_loc_conf_t *prev = parent;
  ngx_http_anch_rest_loc_conf_t *conf = child;

  ngx_conf_merge_bufs_value(conf->bufs, prev->bufs, 4, 16384);
  if (conf->pool_off == NGX_CONF_UNSET) {
    conf->pool_off = prev->pool_off;
    conf->pool_name = prev->pool_name;
  }
  if (conf->pool_off == NGX_CONF_UNSET) {
    conf->pool_off = 0;
  }
  if (conf->app == NULL || conf->pool_off) {
    return NGX_CONF_OK;
  }
#if (NGX_THREADS)
  /* Default thread pool is "default" (32 threads unless declared with thread_pool directive) */
  conf->pool = ngx_thread_pool_add(cf, conf->pool_name.len ? &conf->pool_name : NULL);
  if (conf->pool == NULL) {
    return NGX_CONF_ERROR;
  }
#else
  if (conf->pool_name.len) {
    ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "anch_rest_thread_pool needs nginx built with --with-threads");
    return NGX_CONF_ERROR;
  }
#endif
  return NGX_CONF_OK;
}

static void
ngx_http_anch_rest_close(void *data)
{
  dlclose(data);
}

static char *
ngx_http_anch_rest_app(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
  ngx_http_anch_rest_loc_conf_t   *alcf = conf;
  ngx_http_anch_rest_main_conf_t  *amcf;
  ngx_http_core_loc_conf_t        *clcf;
  ngx_http_anch_rest_app_t       **apps, *app;
  ngx_str_t                       *value, path, config;
  ngx_pool_cleanup_t              *cln;
  anch_rest_application_fn         entry;
  ngx_uint_t                       i;

  (void) cmd;
  if (alcf->app != NULL) {
    return "is duplicate";
  }
  value = cf->args->elts;
  path = value[1];
  if (ngx_conf_full_name(cf->cycle, &path, 0) != NGX_OK) {
    return NGX_CONF_ERROR;
  }
  /* Null terminated configuration */
  config.len = (cf->args->nelts > 2) ? value[2].len : 0;
  config.data = ngx_pnalloc(cf->pool, config.len + 1);
  if (config.data == NULL) {
    return NGX_CONF_ERROR;
  }
  if (config.len) {
    ngx_memcpy(config.data, value[2].data, config.len);
  }
  config.data[config.len] = '\0';

  amcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_anch_rest_module);

  /* Same library and configuration: same application */
  apps = amcf->apps.elts;
  app = NULL;
  for (i = 0; i < amcf->apps.nelts; i++) {
    if (apps[i]->path.len == path.len && ngx_strncmp(apps[i]->path.data, path.data, path.len) == 0
        && apps[i]->config.len == config.len && ngx_strncmp(apps[i]->config.data, config.data, config.len) == 0)
    {
      app = apps[i];
      break;
    }
  }

  if (app == NULL) {
    app = ngx_pcalloc(cf->pool, sizeof(ngx_http_anch_rest_app_t));
    if (app == NULL) {
      return NGX_CONF_ERROR;
    }
    app->path.len = path.len;
    app->path.data = ngx_pnalloc(cf->pool, path.len + 1);
    if (app->path.data == NULL) {
      return NGX_CONF_ERROR;
    }
    ngx_memcpy(app->path.data, path.data, path.len);
    app->path.data[path.len] = '\0';
    app->config = config;

    app->handle = dlopen((const char *) app->path.data, RTLD_NOW | RTLD_LOCAL);
    if (app->handle == NULL) {
      ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "anch_rest_app: %s", dlerror());
      return NGX_CONF_ERROR;
    }
    cln = ngx_pool_cleanup_add(cf->pool, 0);
    if (cln == NULL) {
      dlclose(app->handle);
      return NGX_CONF_ERROR;
    }
    cln->handler = ngx_http_anch_rest_close;
    cln->data = app->handle;

    *(void **) (&entry) = dlsym(app->handle, ANCH_REST_APPLICATION_SYMBOL);
    if (entry == NULL) {
      ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "anch_rest_app: \"%V\" does not export " ANCH_REST_APPLICATION_SYMBOL
                         " (ANCH_REST_APPLICATION)", &app->path);
      return NGX_CONF_ERROR;
    }
    app->ops = entry();
    if (app->ops == NULL || app->ops->abi_version != ANCH_REST_NGINX_ABI_VERSION) {
      ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "anch_rest_app: \"%V\" has an incompatible ABI version", &app->path);
      return NGX_CONF_ERROR;
    }
    apps = ngx_array_push(&amcf->apps);
    if (apps == NULL) {
      return NGX_CONF_ERROR;
    }
    *apps = app;
  }

  alcf->app = app;
  clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
  clcf->handler = ngx_http_anch_rest_handler;
  return NGX_CONF_OK;
}

static char *
ngx_http_anch_rest_thread_pool(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
  ngx_http_anch_rest_loc_conf_t *alcf = conf;
  ngx_str_t                     *value;

  (void) cmd;
  if (alcf->pool_off != NGX_CONF_UNSET) {
    return "is duplicate";
  }
  value = cf->args->elts;
  if (value[1].len == 3 && ngx_strncmp(value[1].data, "off", 3) == 0) {
    alcf->pool_off = 1;
    return NGX_CONF_OK;
  }
#if !(NGX_THREADS)
  ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "anch_rest_thread_pool needs nginx built with --with-threads");
  return NGX_CONF_ERROR;
#else
  alcf->pool_off = 0;
  alcf->pool_name = value[1];
  return NGX_CONF_OK;
#endif
}
/* Configuration - */

/* Process + */
#if (NGX_THREADS)
static void ngx_http_anch_rest_channel_handler(ngx_event_t *ev);

static void
ngx_http_anch_rest_channel_init(ngx_cycle_t *cycle)
{
  ngx_http_anch_rest_channel_t *ch = &ngx_http_anch_rest_channel;

  if (pipe(ch->fds) == -1) {
    ngx_log_error(NGX_LOG_ALERT, cycle->log, ngx_errno, "anch_rest: pipe() failed, thread pool is not used");
    return;
  }
  if (ngx_nonblocking(ch->fds[0]) == -1 || ngx_nonblocking(ch->fds[1]) == -1
      || fcntl(ch->fds[0], F_SETFD, FD_CLOEXEC) == -1 || fcntl(ch->fds[1], F_SETFD, FD_CLOEXEC) == -1
      || ngx_thread_mutex_create(&ch->mutex, cycle->log) != NGX_OK)
  {
    ngx_log_error(NGX_LOG_ALERT, cycle->log, ngx_errno, "anch_rest: notification channel setup failed, thread pool is not used");
    close(ch->fds[0]);
    close(ch->fds[1]);
    return;
  }
  if (ngx_add_channel_event(cycle, ch->fds[0], NGX_READ_EVENT, ngx_http_anch_rest_channel_handler) != NGX_OK) {
    ngx_log_error(NGX_LOG_ALERT, cycle->log, 0, "anch_rest: notification channel registration failed, thread pool is not used");
    close(ch->fds[0]);
    close(ch->fds[1]);
    return;
  }
  ch->head = NULL;
  ch->ready = 1;
}
#endif

static ngx_int_t
ngx_http_anch_rest_init_process(ngx_cycle_t *cycle)
{
  ngx_http_anch_rest_main_conf_t  *amcf;
  ngx_http_anch_rest_app_t       **apps;
  ngx_uint_t                       i;
  char                             error[512];

  amcf = ngx_http_cycle_get_module_main_conf(cycle, ngx_http_anch_rest_module);
  if (amcf == NULL || amcf->apps.nelts == 0) {
    return NGX_OK;
  }
#if (NGX_THREADS)
  ngx_http_anch_rest_channel_init(cycle);
#endif
  apps = amcf->apps.elts;
  for (i = 0; i < amcf->apps.nelts; i++) {
    error[0] = '\0';
    apps[i]->instance = apps[i]->ops->create((const char *) apps[i]->config.data, error, sizeof(error));
    if (apps[i]->instance == NULL) {
      /* Requests are answered with 500 */
      ngx_log_error(NGX_LOG_EMERG, cycle->log, 0, "anch_rest_app \"%V\" creation failed: %s", &apps[i]->path, error);
    }
  }
  return NGX_OK;
}

static void
ngx_http_anch_rest_exit_process(ngx_cycle_t *cycle)
{
  ngx_http_anch_rest_main_conf_t  *amcf;
  ngx_http_anch_rest_app_t       **apps;
  ngx_uint_t                       i;

  amcf = ngx_http_cycle_get_module_main_conf(cycle, ngx_http_anch_rest_module);
  if (amcf == NULL) {
    return;
  }
  apps = amcf->apps.elts;
  for (i = 0; i < amcf->apps.nelts; i++) {
    if (apps[i]->instance != NULL) {
      apps[i]->ops->destroy(apps[i]->instance);
      apps[i]->instance = NULL;
    }
  }
}
/* Process - */

/* Response + */
static ngx_str_t *
ngx_http_anch_rest_copy(ngx_http_request_t *r, ngx_str_t *dst, const anch_rest_str_t *src)
{
  dst->len = src->len;
  dst->data = ngx_pnalloc(r->pool, src->len ? src->len : 1);
  if (dst->data == NULL) {
    return NULL;
  }
  if (src->len) {
    ngx_memcpy(dst->data, src->data, src->len);
  }
  return dst;
}

/* Send status and headers (event loop) */
static ngx_int_t
ngx_http_anch_rest_send_header(ngx_http_anch_rest_ctx_t *ctx, ngx_uint_t status, const anch_rest_header_t *headers,
                               size_t count, long long length)
{
  ngx_http_request_t *r = ctx->request;
  ngx_table_elt_t    *h;
  ngx_str_t           name;
  size_t              i;
  ngx_int_t           rc;

  r->headers_out.status = status;
  for (i = 0; i < count; i++) {
    if (ngx_http_anch_rest_copy(r, &name, &headers[i].name) == NULL) {
      ctx->failed = 1;
      return NGX_ERROR;
    }
    if (name.len == sizeof("Content-Length") - 1 && ngx_strncasecmp(name.data, (u_char *) "Content-Length", name.len) == 0) {
      continue;
    }
    if (name.len == sizeof("Content-Type") - 1 && ngx_strncasecmp(name.data, (u_char *) "Content-Type", name.len) == 0) {
      if (ngx_http_anch_rest_copy(r, &r->headers_out.content_type, &headers[i].value) == NULL) {
        ctx->failed = 1;
        return NGX_ERROR;
      }
      r->headers_out.content_type_len = r->headers_out.content_type.len;
      r->headers_out.content_type_lowcase = NULL;
      continue;
    }
    h = ngx_list_push(&r->headers_out.headers);
    if (h == NULL || ngx_http_anch_rest_copy(r, &h->value, &headers[i].value) == NULL) {
      ctx->failed = 1;
      return NGX_ERROR;
    }
    h->hash = 1;
    h->key = name;
#if (nginx_version >= 1023000)
    h->next = NULL;
#endif
  }
  /* Unknown length: chunked transfer encoding (HTTP/1.1) */
  r->headers_out.content_length_n = (length >= 0) ? (off_t) length : -1;
  if (length == 0) {
    r->header_only = 1;
  }
  rc = ngx_http_send_header(r);
  ctx->header_sent = 1;
  ctx->header_rc = rc;
  ctx->header_only = r->header_only;
  if (rc == NGX_ERROR || rc > NGX_OK) {
    ctx->failed = 1;
  }
  return rc;
}

/* Give data to nginx (event loop): NULL sends pending data */
static void
ngx_http_anch_rest_output(ngx_http_anch_rest_ctx_t *ctx, ngx_chain_t *in)
{
  ngx_http_request_t       *r = ctx->request;
  ngx_connection_t         *c = r->connection;
  ngx_event_t              *wev = c->write;
  ngx_http_core_loc_conf_t *clcf;
  ngx_int_t                 rc;

  if (!ctx->header_sent || ctx->header_only || ctx->failed) {
    return;
  }
  rc = ngx_http_output_filter(r, in);
  if (rc == NGX_ERROR) {
    ctx->failed = 1;
    return;
  }
  clcf = ngx_http_get_module_loc_conf(r->main, ngx_http_core_module);
  if (r->buffered || r->postponed || c->buffered) {
    /* Client is slower: wait for write event (as ngx_http_writer) */
    if (!wev->delayed) {
      ngx_add_timer(wev, clcf->send_timeout);
    }
    if (ngx_handle_write_event(wev, clcf->send_lowat) != NGX_OK) {
      ctx->failed = 1;
    }
  } else if (!wev->delayed && wev->timer_set) {
    ngx_del_timer(wev);
  }
}

/* Send last buffer and return the request finalization code (event loop) */
static ngx_int_t
ngx_http_anch_rest_finish(ngx_http_anch_rest_ctx_t *ctx)
{
  ngx_http_request_t *r = ctx->request;
  ngx_buf_t          *b;
  ngx_chain_t         chain;

  if (!ctx->header_sent) {
    /* Response has not been started (or headers could not be copied) */
    return r->connection->error ? NGX_ERROR : NGX_HTTP_INTERNAL_SERVER_ERROR;
  }
  if (ctx->header_rc == NGX_ERROR || ctx->header_rc > NGX_OK) {
    return ctx->header_rc;
  }
  if (ctx->failed || ctx->result != 0 || r->connection->error) {
    /* Interrupted response: connection is closed */
    return NGX_ERROR;
  }
  if (ctx->header_only) {
    return ctx->header_rc;
  }
  b = ngx_calloc_buf(r->pool);
  if (b == NULL) {
    return NGX_ERROR;
  }
  b->last_buf = (r == r->main) ? 1 : 0;
  b->last_in_chain = 1;
  chain.buf = b;
  chain.next = NULL;
  return ngx_http_output_filter(r, &chain);
}
/* Response - */

/* Request + */
static ngx_int_t
ngx_http_anch_rest_prepare(ngx_http_request_t *r, ngx_http_anch_rest_ctx_t *ctx)
{
  ngx_list_part_t    *part;
  ngx_table_elt_t    *h;
  anch_rest_header_t *headers;
  ngx_uint_t          i, count;

  ctx->request = r;
  ctx->in.method.data = (const char *) r->method_name.data;
  ctx->in.method.len = r->method_name.len;
  ctx->in.target.data = (const char *) r->unparsed_uri.data;
  ctx->in.target.len = r->unparsed_uri.len;

  count = 0;
  for (part = &r->headers_in.headers.part; part != NULL; part = part->next) {
    count += part->nelts;
  }
  headers = ngx_pcalloc(r->pool, (count ? count : 1) * sizeof(anch_rest_header_t));
  if (headers == NULL) {
    return NGX_ERROR;
  }
  count = 0;
  for (part = &r->headers_in.headers.part; part != NULL; part = part->next) {
    h = part->elts;
    for (i = 0; i < part->nelts; i++) {
      if (h[i].hash == 0) {
        continue;
      }
      headers[count].name.data = (const char *) h[i].key.data;
      headers[count].name.len = h[i].key.len;
      headers[count].value.data = (const char *) h[i].value.data;
      headers[count].value.len = h[i].value.len;
      count++;
    }
  }
  ctx->in.headers = headers;
  ctx->in.headers_count = count;
  ctx->in.has_body = (r->headers_in.content_length_n >= 0 || r->headers_in.chunked) ? 1 : 0;
  return NGX_OK;
}
/* Request - */

/* Event loop handler (no thread pool) + */
/* Read buffered request body: memory and temporary file buffers */
static size_t
ngx_http_anch_rest_sync_read(void *data, char *buf, size_t size)
{
  ngx_http_anch_rest_ctx_t *ctx = data;
  ngx_buf_t                *b;
  off_t                     avail;
  size_t                    n;
  ssize_t                   rd;

  while (ctx->in_cl != NULL) {
    b = ctx->in_cl->buf;
    if (ngx_buf_in_memory(b)) {
      avail = (off_t) (b->last - b->pos) - ctx->in_offset;
    } else if (b->in_file) {
      avail = b->file_last - b->file_pos - ctx->in_offset;
    } else {
      avail = 0;
    }
    if (avail <= 0) {
      ctx->in_cl = ctx->in_cl->next;
      ctx->in_offset = 0;
      continue;
    }
    n = ((off_t) size < avail) ? size : (size_t) avail;
    if (ngx_buf_in_memory(b)) {
      ngx_memcpy(buf, b->pos + ctx->in_offset, n);
    } else {
      rd = ngx_read_file(b->file, (u_char *) buf, n, b->file_pos + ctx->in_offset);
      if (rd == NGX_ERROR || rd <= 0) {
        return ANCH_REST_READ_ERROR;
      }
      n = (size_t) rd;
    }
    ctx->in_offset += (off_t) n;
    return n;
  }
  return 0;
}

static int
ngx_http_anch_rest_sync_start(void *data, unsigned int status, const anch_rest_header_t *headers, size_t count,
                              long long length)
{
  ngx_http_anch_rest_ctx_t *ctx = data;

  if (ctx->header_sent || ctx->failed) {
    return -1;
  }
  ngx_http_anch_rest_send_header(ctx, status, headers, count, length);
  return ctx->failed ? -1 : 0;
}

static size_t
ngx_http_anch_rest_sync_write(void *data, const char *buf, size_t len)
{
  ngx_http_anch_rest_ctx_t *ctx = data;
  ngx_buf_t                *b;
  ngx_chain_t               chain;

  if (!ctx->header_sent || ctx->failed) {
    return 0;
  }
  if (ctx->header_only || len == 0) {
    return len;
  }
  b = ngx_create_temp_buf(ctx->request->pool, len);
  if (b == NULL) {
    ctx->failed = 1;
    return 0;
  }
  b->last = ngx_cpymem(b->last, buf, len);
  chain.buf = b;
  chain.next = NULL;
  ngx_http_anch_rest_output(ctx, &chain);
  return ctx->failed ? 0 : len;
}

static void
ngx_http_anch_rest_sync_handle(ngx_http_request_t *r, ngx_http_anch_rest_loc_conf_t *alcf)
{
  ngx_http_anch_rest_ctx_t *ctx;

  ctx = ngx_pcalloc(r->pool, sizeof(ngx_http_anch_rest_ctx_t));
  if (ctx == NULL || ngx_http_anch_rest_prepare(r, ctx) != NGX_OK) {
    ngx_http_finalize_request(r, NGX_HTTP_INTERNAL_SERVER_ERROR);
    return;
  }
  ctx->app = alcf->app;
  ctx->in_cl = (r->request_body != NULL) ? r->request_body->bufs : NULL;
  ctx->in.body.ctx = ctx;
  ctx->in.body.read = ngx_http_anch_rest_sync_read;
  ctx->writer.ctx = ctx;
  ctx->writer.buffer_size = alcf->bufs.size;
  ctx->writer.start = ngx_http_anch_rest_sync_start;
  ctx->writer.write = ngx_http_anch_rest_sync_write;
  ctx->result = ctx->app->ops->handle(ctx->app->instance, &ctx->in, &ctx->writer);
  ngx_http_finalize_request(r, ngx_http_anch_rest_finish(ctx));
}
/* Event loop handler (no thread pool) - */

#if (NGX_THREADS)
/* Thread pool handler + */
static void ngx_http_anch_rest_pump(ngx_http_anch_rest_ctx_t *ctx);

/* Wake the event loop up for context (handler thread) */
static void
ngx_http_anch_rest_notify(ngx_http_anch_rest_ctx_t *ctx)
{
  ngx_http_anch_rest_channel_t *ch = &ngx_http_anch_rest_channel;
  ngx_uint_t                    wake = 0;

  (void) ngx_thread_mutex_lock(&ch->mutex, ctx->log);
  if (ctx->queued == 0) {
    ctx->queued = 1;
    ctx->ready_next = ch->head;
    wake = (ch->head == NULL);
    ch->head = ctx;
  } else if (ctx->queued == 2) {
    ctx->queued = 3; /* processed by event loop: queued again after processing */
  }
  (void) ngx_thread_mutex_unlock(&ch->mutex, ctx->log);
  if (wake && write(ch->fds[1], "", 1) == -1) {
    /* Full pipe: a notification is already pending */
  }
}

/* Process notified contexts (event loop) */
static void
ngx_http_anch_rest_channel_handler(ngx_event_t *ev)
{
  ngx_http_anch_rest_channel_t *ch = &ngx_http_anch_rest_channel;
  ngx_http_anch_rest_ctx_t     *list, *ctx;
  ngx_connection_t             *c;
  ngx_uint_t                    wake;
  u_char                        buf[64];

  (void) ev;
  /* Pipe is read before list is taken: a notification sent after is not lost */
  while (read(ch->fds[0], buf, sizeof(buf)) > 0) {
    /* void */
  }
  (void) ngx_thread_mutex_lock(&ch->mutex, ngx_cycle->log);
  list = ch->head;
  ch->head = NULL;
  for (ctx = list; ctx != NULL; ctx = ctx->ready_next) {
    ctx->queued = 2; /* ready_next is not modified by notify while processing */
  }
  (void) ngx_thread_mutex_unlock(&ch->mutex, ngx_cycle->log);

  while (list != NULL) {
    ctx = list;
    list = ctx->ready_next;
    c = ctx->request->connection;
    ngx_http_set_log_request(c->log, ctx->request);
    ngx_http_anch_rest_pump(ctx);
    wake = 0;
    (void) ngx_thread_mutex_lock(&ch->mutex, ngx_cycle->log);
    if (ctx->queued == 3) {
      ctx->queued = 1;
      ctx->ready_next = ch->head;
      wake = (ch->head == NULL);
      ch->head = ctx;
    } else {
      ctx->queued = 0;
    }
    (void) ngx_thread_mutex_unlock(&ch->mutex, ngx_cycle->log);
    if (wake && write(ch->fds[1], "", 1) == -1) {
      /* Full pipe: a notification is already pending */
    }
    ngx_http_run_posted_requests(c);
  }
}

/* Remove context from notification list when handler is done (event loop) */
static void
ngx_http_anch_rest_channel_remove(ngx_http_anch_rest_ctx_t *ctx)
{
  ngx_http_anch_rest_channel_t *ch = &ngx_http_anch_rest_channel;
  ngx_http_anch_rest_ctx_t    **p;

  (void) ngx_thread_mutex_lock(&ch->mutex, ngx_cycle->log);
  if (ctx->queued) {
    for (p = &ch->head; *p != NULL; p = &(*p)->ready_next) {
      if (*p == ctx) {
        *p = ctx->ready_next;
        break;
      }
    }
    ctx->queued = 0;
  }
  (void) ngx_thread_mutex_unlock(&ch->mutex, ngx_cycle->log);
}

/* Read request body (handler thread) */
static size_t
ngx_http_anch_rest_thread_read(void *data, char *buf, size_t size)
{
  ngx_http_anch_rest_ctx_t *ctx = data;
  ngx_http_anch_rest_seg_t *seg;
  ngx_uint_t                notify = 0;
  size_t                    n;

  (void) ngx_thread_mutex_lock(&ctx->mutex, ctx->log);
  for ( ;; ) {
    seg = ctx->in_head;
    if (seg != NULL) {
      n = (size_t) (seg->last - seg->pos);
      if (n > size) {
        n = size;
      }
      ngx_memcpy(buf, seg->pos, n);
      seg->pos += n;
      if (seg->pos == seg->last) {
        /* Segment can be released: nginx reads more data */
        ctx->in_head = seg->next;
        notify = 1;
      }
      break;
    }
    if (ctx->cancelled || ctx->in_error) {
      n = ANCH_REST_READ_ERROR;
      break;
    }
    if (ctx->in_eof) {
      n = 0;
      break;
    }
    (void) ngx_thread_cond_wait(&ctx->cond, &ctx->mutex, ctx->log);
  }
  (void) ngx_thread_mutex_unlock(&ctx->mutex, ctx->log);
  if (notify) {
    ngx_http_anch_rest_notify(ctx);
  }
  return n;
}

/* Start response (handler thread): headers are copied for the event loop */
static int
ngx_http_anch_rest_thread_start(void *data, unsigned int status, const anch_rest_header_t *headers, size_t count,
                                long long length)
{
  ngx_http_anch_rest_ctx_t *ctx = data;
  anch_rest_header_t       *copy;
  size_t                    size, i;
  char                     *p;
  int                       rc = 0;

  size = count * sizeof(anch_rest_header_t);
  for (i = 0; i < count; i++) {
    size += headers[i].name.len + headers[i].value.len;
  }
  copy = malloc(size ? size : 1);
  if (copy == NULL) {
    return -1;
  }
  p = (char *) (copy + count);
  for (i = 0; i < count; i++) {
    copy[i].name.data = p;
    copy[i].name.len = headers[i].name.len;
    if (headers[i].name.len) {
      p = (char *) ngx_cpymem(p, headers[i].name.data, headers[i].name.len);
    }
    copy[i].value.data = p;
    copy[i].value.len = headers[i].value.len;
    if (headers[i].value.len) {
      p = (char *) ngx_cpymem(p, headers[i].value.data, headers[i].value.len);
    }
  }
  (void) ngx_thread_mutex_lock(&ctx->mutex, ctx->log);
  if (ctx->cancelled || ctx->started) {
    rc = -1;
  } else {
    ctx->started = 1;
    ctx->start_pending = 1;
    ctx->start_status = status;
    ctx->start_headers = copy;
    ctx->start_count = count;
    ctx->start_length = length;
    copy = NULL;
  }
  (void) ngx_thread_mutex_unlock(&ctx->mutex, ctx->log);
  free(copy);
  if (rc == 0) {
    ngx_http_anch_rest_notify(ctx);
  }
  return rc;
}

/* Write response body (handler thread): waits for a free buffer */
static size_t
ngx_http_anch_rest_thread_write(void *data, const char *buf, size_t len)
{
  ngx_http_anch_rest_ctx_t *ctx = data;
  ngx_http_anch_rest_out_t *o;
  ngx_uint_t                notify = 0;
  size_t                    n = 0;

  (void) ngx_thread_mutex_lock(&ctx->mutex, ctx->log);
  for ( ;; ) {
    if (ctx->cancelled) {
      n = 0;
      break;
    }
    if (ctx->out_current < 0) {
      if (ctx->out_free_count == 0) {
        if (ctx->out_count < ctx->out_max && !ctx->out_want) {
          /* Ask the event loop for a new buffer */
          ctx->out_want = 1;
          (void) ngx_thread_mutex_unlock(&ctx->mutex, ctx->log);
          ngx_http_anch_rest_notify(ctx);
          (void) ngx_thread_mutex_lock(&ctx->mutex, ctx->log);
          continue;
        }
        (void) ngx_thread_cond_wait(&ctx->cond, &ctx->mutex, ctx->log);
        continue;
      }
      ctx->out_current = (ngx_int_t) ctx->out_free[--ctx->out_free_count];
      ctx->out[ctx->out_current].len = 0;
    }
    o = &ctx->out[ctx->out_current];
    n = ctx->out_size - o->len;
    if (n > len) {
      n = len;
    }
    ngx_memcpy(o->buf->start + o->len, buf, n);
    o->len += n;
    if (o->len == ctx->out_size) {
      ctx->out_full[ctx->out_full_count++] = (ngx_uint_t) ctx->out_current;
      ctx->out_current = -1;
      notify = 1;
    }
    break;
  }
  (void) ngx_thread_mutex_unlock(&ctx->mutex, ctx->log);
  if (notify) {
    ngx_http_anch_rest_notify(ctx);
  }
  return n;
}

/* Execute application handler (handler thread) */
static void
ngx_http_anch_rest_thread_handler(void *data, ngx_log_t *log)
{
  ngx_http_anch_rest_ctx_t *ctx = data;
  int                       result;

  (void) log;
  result = ctx->app->ops->handle(ctx->app->instance, &ctx->in, &ctx->writer);
  (void) ngx_thread_mutex_lock(&ctx->mutex, ctx->log);
  if (ctx->out_current >= 0) {
    /* Last buffer */
    if (ctx->out[ctx->out_current].len > 0) {
      ctx->out_full[ctx->out_full_count++] = (ngx_uint_t) ctx->out_current;
    } else {
      ctx->out_free[ctx->out_free_count++] = (ngx_uint_t) ctx->out_current;
    }
    ctx->out_current = -1;
  }
  ctx->result = result;
  (void) ngx_thread_mutex_unlock(&ctx->mutex, ctx->log);
}

/* Allocate a response buffer (event loop) */
static ngx_int_t
ngx_http_anch_rest_out_alloc(ngx_http_anch_rest_ctx_t *ctx, ngx_uint_t *index)
{
  ngx_http_anch_rest_out_t *o = &ctx->out[ctx->out_count];

  o->buf = ngx_create_temp_buf(ctx->request->pool, ctx->out_size);
  if (o->buf == NULL) {
    return NGX_ERROR;
  }
  o->buf->tag = (ngx_buf_tag_t) &ngx_http_anch_rest_module;
  o->buf->flush = 1; /* filters must not wait for more data: the handler may wait for this buffer */
  o->chain.buf = o->buf;
  o->chain.next = NULL;
  *index = ctx->out_count;
  return NGX_OK;
}

/* Take new request body buffers (event loop) */
static ngx_int_t
ngx_http_anch_rest_take_body(ngx_http_anch_rest_ctx_t *ctx, ngx_http_anch_rest_seg_t **first,
                             ngx_http_anch_rest_seg_t **last)
{
  ngx_http_request_t       *r = ctx->request;
  ngx_http_anch_rest_seg_t *seg;
  ngx_chain_t              *cl, *next;
  ngx_buf_t                *b;

  *first = NULL;
  *last = NULL;
  if (r->request_body == NULL) {
    return NGX_OK;
  }
  for (cl = r->request_body->bufs; cl != NULL; cl = next) {
    next = cl->next;
    b = cl->buf;
    if (ngx_buf_in_memory(b) && b->last > b->pos) {
      seg = ngx_palloc(r->pool, sizeof(ngx_http_anch_rest_seg_t));
      if (seg == NULL) {
        return NGX_ERROR;
      }
      seg->next = NULL;
      seg->buf = b;
      seg->pos = b->pos;
      seg->last = b->last;
      if (*last == NULL) {
        *first = seg;
      } else {
        (*last)->next = seg;
      }
      *last = seg;
    } else if (b->in_file && b->file_last > b->file_pos) {
      ngx_log_error(NGX_LOG_ALERT, r->connection->log, 0, "anch_rest: unexpected request body in file");
      return NGX_ERROR;
    }
    ngx_free_chain(r->pool, cl);
  }
  r->request_body->bufs = NULL;
  return NGX_OK;
}

/* Exchange data with handler thread (event loop) */
static void
ngx_http_anch_rest_pump(ngx_http_anch_rest_ctx_t *ctx)
{
  ngx_http_request_t       *r = ctx->request;
  ngx_http_anch_rest_seg_t *seg, *first = NULL, *last = NULL;
  ngx_http_anch_rest_out_t *o;
  ngx_chain_t              *out = NULL, **ll = &out;
  anch_rest_header_t       *headers = NULL;
  ngx_uint_t                i, nfull, nfreed = 0, cancelled, want, start, status = 0, eof = 0, error = 0;
  ngx_int_t                 rc, in_status = 0;
  size_t                    count = 0;
  long long                 length = 0;

  /* Take shared state + */
  (void) ngx_thread_mutex_lock(&ctx->mutex, ctx->log);
  cancelled = ctx->cancelled;
  start = ctx->start_pending;
  if (start) {
    ctx->start_pending = 0;
    status = ctx->start_status;
    headers = ctx->start_headers;
    count = ctx->start_count;
    length = ctx->start_length;
    ctx->start_headers = NULL;
    in_status = ctx->in_status;
  }
  nfull = ctx->out_full_count;
  for (i = 0; i < nfull; i++) {
    ctx->sendq[i] = ctx->out_full[i];
  }
  ctx->out_full_count = 0;
  want = ctx->out_want;
  /* Read segments are released: nginx can reuse their buffers */
  for (seg = ctx->in_release; seg != NULL && seg != ctx->in_head; seg = seg->next) {
    seg->buf->pos = seg->buf->last;
  }
  ctx->in_release = seg;
  (void) ngx_thread_mutex_unlock(&ctx->mutex, ctx->log);
  /* Take shared state - */

  if (cancelled) {
    ctx->failed = 1;
  }

  /* Response start + */
  if (start) {
    if (!ctx->failed) {
      /* Reading error with HTTP status (413, 408, ...) is more accurate than application error */
      if (in_status && status >= NGX_HTTP_BAD_REQUEST) {
        status = (ngx_uint_t) in_status;
      }
      ngx_http_anch_rest_send_header(ctx, status, headers, count, length);
    }
    free(headers);
  }
  /* Response start - */

  /* Request body + */
  if (ctx->reading && !ctx->failed) {
    rc = ngx_http_read_unbuffered_request_body(r);
    if (rc == NGX_ERROR || rc >= NGX_HTTP_SPECIAL_RESPONSE) {
      error = 1;
      in_status = (rc >= NGX_HTTP_SPECIAL_RESPONSE) ? rc : NGX_HTTP_INTERNAL_SERVER_ERROR;
    }
    if (ngx_http_anch_rest_take_body(ctx, &first, &last) != NGX_OK) {
      error = 1;
      in_status = NGX_HTTP_INTERNAL_SERVER_ERROR;
    }
    if (error || !r->reading_body) {
      eof = !error;
      ctx->reading = 0;
      r->read_event_handler = ngx_http_block_reading;
    }
  }
  /* Request body - */

  /* Response body + */
  for (i = 0; i < nfull; i++) {
    o = &ctx->out[ctx->sendq[i]];
    o->buf->pos = o->buf->start;
    o->buf->last = o->buf->start + o->len;
    o->sending = 1;
    o->chain.next = NULL;
    *ll = &o->chain;
    ll = &o->chain.next;
  }
  ngx_http_anch_rest_output(ctx, out);
  /* Sent buffers, or dropped ones (HEAD, failure), are given back to the handler */
  for (i = 0; i < ctx->out_count; i++) {
    o = &ctx->out[i];
    if (o->sending && (o->buf->pos == o->buf->last || ctx->failed || ctx->header_only || !ctx->header_sent)) {
      o->sending = 0;
      ctx->freeq[nfreed++] = i;
    }
  }
  if (want && ctx->out_count < ctx->out_max && !ctx->failed) {
    if (ngx_http_anch_rest_out_alloc(ctx, &i) == NGX_OK) {
      ctx->freeq[nfreed++] = i;
    } else {
      ctx->failed = 1;
    }
  } else {
    want = 0;
  }
  /* Response body - */

  /* Give shared state + */
  (void) ngx_thread_mutex_lock(&ctx->mutex, ctx->log);
  if (first != NULL) {
    if (ctx->in_tail != NULL) {
      ctx->in_tail->next = first;
    }
    ctx->in_tail = last;
    if (ctx->in_head == NULL) {
      ctx->in_head = first;
    }
    if (ctx->in_release == NULL) {
      ctx->in_release = first;
    }
  }
  if (eof) {
    ctx->in_eof = 1;
  }
  if (error) {
    ctx->in_error = 1;
    ctx->in_status = in_status;
  }
  if (want) {
    ctx->out_count++;
    ctx->out_want = 0;
  }
  for (i = 0; i < nfreed; i++) {
    ctx->out_free[ctx->out_free_count++] = ctx->freeq[i];
  }
  if (ctx->failed) {
    ctx->cancelled = 1;
  }
  (void) ngx_thread_cond_signal(&ctx->cond, ctx->log);
  (void) ngx_thread_mutex_unlock(&ctx->mutex, ctx->log);
  /* Give shared state - */
}

/* Request terminated by nginx (client closed connection, worker shutdown, ...): handler is cancelled */
static void
ngx_http_anch_rest_cleanup(void *data)
{
  ngx_http_anch_rest_ctx_t *ctx = data;

  (void) ngx_thread_mutex_lock(&ctx->mutex, ctx->log);
  ctx->cancelled = 1;
  (void) ngx_thread_cond_signal(&ctx->cond, ctx->log);
  (void) ngx_thread_mutex_unlock(&ctx->mutex, ctx->log);
}

/* Request pool destruction: synchronization primitives (after HTTP cleanups) */
static void
ngx_http_anch_rest_pool_cleanup(void *data)
{
  ngx_http_anch_rest_ctx_t *ctx = data;

  free(ctx->start_headers);
  ctx->start_headers = NULL;
  (void) ngx_thread_cond_destroy(&ctx->cond, ctx->log);
  (void) ngx_thread_mutex_destroy(&ctx->mutex, ctx->log);
}

static void
ngx_http_anch_rest_read_handler(ngx_http_request_t *r)
{
  ngx_http_anch_rest_pump(ngx_http_get_module_ctx(r, ngx_http_anch_rest_module));
}

static void
ngx_http_anch_rest_write_handler(ngx_http_request_t *r)
{
  ngx_http_anch_rest_ctx_t *ctx = ngx_http_get_module_ctx(r, ngx_http_anch_rest_module);
  ngx_connection_t         *c = r->connection;
  ngx_event_t              *wev = c->write;
  ngx_http_core_loc_conf_t *clcf;

  if (wev->timedout) {
    ngx_log_error(NGX_LOG_INFO, c->log, NGX_ETIMEDOUT, "client timed out");
    c->timedout = 1;
    ctx->failed = 1;
  } else if (wev->delayed) {
    /* limit_rate */
    clcf = ngx_http_get_module_loc_conf(r->main, ngx_http_core_module);
    if (ngx_handle_write_event(wev, clcf->send_lowat) != NGX_OK) {
      ctx->failed = 1;
    } else {
      return;
    }
  }
  ngx_http_anch_rest_pump(ctx);
}

/* Handler is done (event loop) */
static void
ngx_http_anch_rest_thread_done(ngx_event_t *ev)
{
  ngx_http_anch_rest_ctx_t *ctx = ev->data;
  ngx_http_request_t       *r = ctx->request;
  ngx_connection_t         *c = r->connection;

  ngx_http_set_log_request(c->log, r);
  ngx_http_anch_rest_channel_remove(ctx);
  ngx_http_anch_rest_pump(ctx); /* last start and buffers */
  r->main->blocked--;
  r->read_event_handler = ngx_http_block_reading; /* body which has not been read: lingering close */
  if (r->main->terminated) {
    /* Terminated by nginx while handler was running: nginx finalizer (as for nginx thread tasks) */
    r->write_event_handler(r);
  } else {
    r->write_event_handler = ngx_http_request_empty_handler;
    ngx_http_finalize_request(r, ngx_http_anch_rest_finish(ctx));
  }
  ngx_http_run_posted_requests(c);
}

/* Start handler in thread pool (event loop) */
static ngx_int_t
ngx_http_anch_rest_thread_handle(ngx_http_request_t *r, ngx_http_anch_rest_loc_conf_t *alcf)
{
  ngx_thread_task_t        *task;
  ngx_http_anch_rest_ctx_t *ctx;
  ngx_pool_cleanup_t       *pcln;
  ngx_http_cleanup_t       *hcln;
  ngx_uint_t                index;

  task = ngx_thread_task_alloc(r->pool, sizeof(ngx_http_anch_rest_ctx_t));
  if (task == NULL) {
    return NGX_ERROR;
  }
  ctx = task->ctx;
  ctx->app = alcf->app;
  ctx->log = ngx_cycle->log;
  if (ngx_http_anch_rest_prepare(r, ctx) != NGX_OK) {
    return NGX_ERROR;
  }

  /* Synchronization +  */
  pcln = ngx_pool_cleanup_add(r->pool, 0);
  if (pcln == NULL) {
    return NGX_ERROR;
  }
  if (ngx_thread_mutex_create(&ctx->mutex, r->connection->log) != NGX_OK) {
    return NGX_ERROR;
  }
  if (ngx_thread_cond_create(&ctx->cond, r->connection->log) != NGX_OK) {
    (void) ngx_thread_mutex_destroy(&ctx->mutex, r->connection->log);
    return NGX_ERROR;
  }
  pcln->handler = ngx_http_anch_rest_pool_cleanup;
  pcln->data = ctx;
  ctx->threaded = 1;
  hcln = ngx_http_cleanup_add(r, 0);
  if (hcln == NULL) {
    return NGX_ERROR;
  }
  hcln->handler = ngx_http_anch_rest_cleanup;
  hcln->data = ctx;
  /* Synchronization -  */

  /* Response buffers (one allocated, others on demand) */
  ctx->out_max = (ngx_uint_t) alcf->bufs.num;
  ctx->out_size = alcf->bufs.size;
  ctx->out = ngx_pcalloc(r->pool, ctx->out_max * sizeof(ngx_http_anch_rest_out_t));
  ctx->out_free = ngx_palloc(r->pool, ctx->out_max * sizeof(ngx_uint_t));
  ctx->out_full = ngx_palloc(r->pool, ctx->out_max * sizeof(ngx_uint_t));
  ctx->sendq = ngx_palloc(r->pool, ctx->out_max * sizeof(ngx_uint_t));
  ctx->freeq = ngx_palloc(r->pool, (ctx->out_max + 1) * sizeof(ngx_uint_t));
  if (ctx->out == NULL || ctx->out_free == NULL || ctx->out_full == NULL || ctx->sendq == NULL || ctx->freeq == NULL
      || ngx_http_anch_rest_out_alloc(ctx, &index) != NGX_OK)
  {
    return NGX_ERROR;
  }
  ctx->out_count = 1;
  ctx->out_free[ctx->out_free_count++] = index;
  ctx->out_current = -1;

  /* Request body already received */
  ctx->reading = r->reading_body ? 1 : 0;
  if (ngx_http_anch_rest_take_body(ctx, &ctx->in_head, &ctx->in_tail) != NGX_OK) {
    return NGX_ERROR;
  }
  ctx->in_release = ctx->in_head;
  ctx->in_eof = !ctx->reading;

  ctx->in.body.ctx = ctx;
  ctx->in.body.read = ngx_http_anch_rest_thread_read;
  ctx->writer.ctx = ctx;
  ctx->writer.buffer_size = ctx->out_size;
  ctx->writer.start = ngx_http_anch_rest_thread_start;
  ctx->writer.write = ngx_http_anch_rest_thread_write;

  ngx_http_set_ctx(r, ctx, ngx_http_anch_rest_module);
  r->read_event_handler = ctx->reading ? ngx_http_anch_rest_read_handler : ngx_http_block_reading;
  r->write_event_handler = ngx_http_anch_rest_write_handler;

  task->handler = ngx_http_anch_rest_thread_handler;
  task->event.data = ctx;
  task->event.handler = ngx_http_anch_rest_thread_done;
  if (ngx_thread_task_post(alcf->pool, task) != NGX_OK) {
    return NGX_ERROR;
  }
  r->main->blocked++;
  return NGX_OK;
}
/* Thread pool handler - */
#endif

static ngx_flag_t
ngx_http_anch_rest_threaded(ngx_http_anch_rest_loc_conf_t *alcf)
{
#if (NGX_THREADS)
  return alcf->pool != NULL && ngx_http_anch_rest_channel.ready;
#else
  (void) alcf;
  return 0;
#endif
}

static void
ngx_http_anch_rest_body_handler(ngx_http_request_t *r)
{
  ngx_http_anch_rest_loc_conf_t *alcf;

  alcf = ngx_http_get_module_loc_conf(r, ngx_http_anch_rest_module);
#if (NGX_THREADS)
  if (ngx_http_anch_rest_threaded(alcf)) {
    if (ngx_http_anch_rest_thread_handle(r, alcf) != NGX_OK) {
      ngx_http_finalize_request(r, NGX_HTTP_INTERNAL_SERVER_ERROR);
    }
    return;
  }
#endif
  /* No thread pool: handler is executed in the event loop */
  ngx_http_anch_rest_sync_handle(r, alcf);
}

static ngx_int_t
ngx_http_anch_rest_handler(ngx_http_request_t *r)
{
  ngx_http_anch_rest_loc_conf_t *alcf;
  ngx_int_t                      rc;

  alcf = ngx_http_get_module_loc_conf(r, ngx_http_anch_rest_module);
  if (alcf->app == NULL) {
    return NGX_DECLINED;
  }
  if (alcf->app->instance == NULL) {
    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "anch_rest_app \"%V\" is not available", &alcf->app->path);
    return NGX_HTTP_INTERNAL_SERVER_ERROR;
  }
  /* In a thread pool, request body is read by the handler while nginx receives it */
  r->request_body_no_buffering = ngx_http_anch_rest_threaded(alcf) ? 1 : 0;
  rc = ngx_http_read_client_request_body(r, ngx_http_anch_rest_body_handler);
  if (rc >= NGX_HTTP_SPECIAL_RESPONSE) {
    return rc;
  }
  return NGX_DONE;
}
