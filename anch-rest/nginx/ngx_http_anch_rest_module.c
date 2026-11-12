/*
  ANCH Framework: ANother C++ Hack is a C++ framework based on C++11 standard
  Copyright (C) 2012 Vincent Lachenal

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
  along with ANCH Framework.  If not, see <http://www.gnu.org/licenses/>.
*/
/*
 * AnCH REST nginx module.
 *
 * Directives (location):
 *   anch_rest_app <library.so> [configuration];   serve location with an AnCH REST application
 *   anch_rest_thread_pool <name>|off;             thread pool for handlers (default: "default")
 *
 * Applications are loaded with dlopen when configuration is read and created in each worker process.
 * Handlers are executed in an nginx thread pool (nginx built with --with-threads), or in the event loop
 * otherwise (blocking).
 */
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

#include <dlfcn.h>

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
#if (NGX_THREADS)
  ngx_thread_pool_t        *pool;
#endif
} ngx_http_anch_rest_loc_conf_t;

typedef struct {
  ngx_http_request_t       *request;
  ngx_http_anch_rest_app_t *app;
  anch_rest_request_t       in;
  anch_rest_response_t      out;
} ngx_http_anch_rest_ctx_t;
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
  alcf->app = NULL;
  alcf->pool_off = NGX_CONF_UNSET;
  return alcf;
}

static char *
ngx_http_anch_rest_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child)
{
  ngx_http_anch_rest_loc_conf_t *prev = parent;
  ngx_http_anch_rest_loc_conf_t *conf = child;

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
static ngx_int_t
ngx_http_anch_rest_init_process(ngx_cycle_t *cycle)
{
  ngx_http_anch_rest_main_conf_t  *amcf;
  ngx_http_anch_rest_app_t       **apps;
  ngx_uint_t                       i;
  char                             error[512];

  amcf = ngx_http_cycle_get_module_main_conf(cycle, ngx_http_anch_rest_module);
  if (amcf == NULL) {
    return NGX_OK;
  }
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

/* Request + */
static ngx_int_t
ngx_http_anch_rest_prepare(ngx_http_request_t *r, ngx_http_anch_rest_ctx_t *ctx)
{
  ngx_list_part_t    *part;
  ngx_table_elt_t    *h;
  anch_rest_header_t *headers;
  ngx_chain_t        *cl;
  ngx_buf_t          *b;
  ngx_uint_t          i, count;
  size_t              len, size;
  u_char             *p;
  ssize_t             n;

  ctx->request = r;
  ctx->in.method.data = (const char *) r->method_name.data;
  ctx->in.method.len = r->method_name.len;
  ctx->in.target.data = (const char *) r->unparsed_uri.data;
  ctx->in.target.len = r->unparsed_uri.len;

  /* Headers */
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

  /* Body (in memory or in temporary file) */
  ctx->in.has_body = (r->headers_in.content_length_n >= 0 || r->headers_in.chunked) ? 1 : 0;
  if (r->request_body == NULL || r->request_body->bufs == NULL) {
    return NGX_OK;
  }
  len = 0;
  for (cl = r->request_body->bufs; cl; cl = cl->next) {
    b = cl->buf;
    len += ngx_buf_in_memory(b) ? (size_t) (b->last - b->pos) : (size_t) (b->file_last - b->file_pos);
  }
  if (len == 0) {
    return NGX_OK;
  }
  p = ngx_pnalloc(r->pool, len);
  if (p == NULL) {
    return NGX_ERROR;
  }
  ctx->in.body.data = (const char *) p;
  ctx->in.body.len = len;
  for (cl = r->request_body->bufs; cl; cl = cl->next) {
    b = cl->buf;
    if (ngx_buf_in_memory(b)) {
      p = ngx_cpymem(p, b->pos, b->last - b->pos);
    } else {
      size = (size_t) (b->file_last - b->file_pos);
      n = ngx_read_file(b->file, p, size, b->file_pos);
      if (n == NGX_ERROR || (size_t) n != size) {
        return NGX_ERROR;
      }
      p += size;
    }
  }
  return NGX_OK;
}

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

static ngx_int_t
ngx_http_anch_rest_send(ngx_http_request_t *r, ngx_http_anch_rest_ctx_t *ctx)
{
  anch_rest_response_t *out = &ctx->out;
  ngx_table_elt_t      *h;
  ngx_buf_t            *b;
  ngx_chain_t           chain;
  ngx_str_t             name;
  ngx_uint_t            i;
  ngx_int_t             rc;

  r->headers_out.status = out->status;
  b = NULL;
  rc = NGX_OK;
  for (i = 0; i < out->headers_count && rc == NGX_OK; i++) {
    if (ngx_http_anch_rest_copy(r, &name, &out->headers[i].name) == NULL) {
      rc = NGX_ERROR;
      break;
    }
    if (name.len == sizeof("Content-Length") - 1 && ngx_strncasecmp(name.data, (u_char *) "Content-Length", name.len) == 0) {
      continue;
    }
    if (name.len == sizeof("Content-Type") - 1 && ngx_strncasecmp(name.data, (u_char *) "Content-Type", name.len) == 0) {
      if (ngx_http_anch_rest_copy(r, &r->headers_out.content_type, &out->headers[i].value) == NULL) {
        rc = NGX_ERROR;
      }
      r->headers_out.content_type_len = r->headers_out.content_type.len;
      r->headers_out.content_type_lowcase = NULL;
      continue;
    }
    h = ngx_list_push(&r->headers_out.headers);
    if (h == NULL || ngx_http_anch_rest_copy(r, &h->value, &out->headers[i].value) == NULL) {
      rc = NGX_ERROR;
      break;
    }
    h->hash = 1;
    h->key = name;
#if (nginx_version >= 1023000)
    h->next = NULL;
#endif
  }
  if (rc == NGX_OK && out->body.len) {
    b = ngx_create_temp_buf(r->pool, out->body.len);
    if (b == NULL) {
      rc = NGX_ERROR;
    } else {
      b->last = ngx_cpymem(b->last, out->body.data, out->body.len);
    }
  }
  r->headers_out.content_length_n = (off_t) out->body.len;
  ctx->app->ops->release(out);
  if (rc != NGX_OK) {
    return NGX_HTTP_INTERNAL_SERVER_ERROR;
  }

  if (b == NULL) {
    r->header_only = 1;
  }
  rc = ngx_http_send_header(r);
  if (rc == NGX_ERROR || rc > NGX_OK || r->header_only) {
    return rc;
  }
  b->last_buf = (r == r->main) ? 1 : 0;
  b->last_in_chain = 1;
  chain.buf = b;
  chain.next = NULL;
  return ngx_http_output_filter(r, &chain);
}

#if (NGX_THREADS)
static void
ngx_http_anch_rest_thread_handler(void *data, ngx_log_t *log)
{
  ngx_http_anch_rest_ctx_t *ctx = data;

  (void) log;
  ctx->app->ops->handle(ctx->app->instance, &ctx->in, &ctx->out);
}

static void
ngx_http_anch_rest_thread_done(ngx_event_t *ev)
{
  ngx_http_anch_rest_ctx_t *ctx = ev->data;
  ngx_http_request_t       *r = ctx->request;
  ngx_connection_t         *c = r->connection;

  ngx_http_set_log_request(c->log, r);
  r->main->blocked--;
  r->aio = 0;
  ngx_http_finalize_request(r, ngx_http_anch_rest_send(r, ctx));
  ngx_http_run_posted_requests(c);
}
#endif

static void
ngx_http_anch_rest_body_handler(ngx_http_request_t *r)
{
  ngx_http_anch_rest_loc_conf_t *alcf;
  ngx_http_anch_rest_ctx_t      *ctx;
#if (NGX_THREADS)
  ngx_thread_task_t             *task;
#endif

  alcf = ngx_http_get_module_loc_conf(r, ngx_http_anch_rest_module);

#if (NGX_THREADS)
  if (alcf->pool != NULL) {
    task = ngx_thread_task_alloc(r->pool, sizeof(ngx_http_anch_rest_ctx_t));
    if (task == NULL) {
      ngx_http_finalize_request(r, NGX_HTTP_INTERNAL_SERVER_ERROR);
      return;
    }
    ctx = task->ctx;
    ctx->app = alcf->app;
    if (ngx_http_anch_rest_prepare(r, ctx) != NGX_OK) {
      ngx_http_finalize_request(r, NGX_HTTP_INTERNAL_SERVER_ERROR);
      return;
    }
    task->handler = ngx_http_anch_rest_thread_handler;
    task->event.data = ctx;
    task->event.handler = ngx_http_anch_rest_thread_done;
    if (ngx_thread_task_post(alcf->pool, task) != NGX_OK) {
      ngx_http_finalize_request(r, NGX_HTTP_INTERNAL_SERVER_ERROR);
      return;
    }
    r->main->blocked++;
    r->aio = 1;
    return;
  }
#endif

  /* No thread pool: handler is executed in the event loop */
  ctx = ngx_pcalloc(r->pool, sizeof(ngx_http_anch_rest_ctx_t));
  if (ctx == NULL) {
    ngx_http_finalize_request(r, NGX_HTTP_INTERNAL_SERVER_ERROR);
    return;
  }
  ctx->app = alcf->app;
  if (ngx_http_anch_rest_prepare(r, ctx) != NGX_OK) {
    ngx_http_finalize_request(r, NGX_HTTP_INTERNAL_SERVER_ERROR);
    return;
  }
  ctx->app->ops->handle(ctx->app->instance, &ctx->in, &ctx->out);
  ngx_http_finalize_request(r, ngx_http_anch_rest_send(r, ctx));
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
  rc = ngx_http_read_client_request_body(r, ngx_http_anch_rest_body_handler);
  if (rc >= NGX_HTTP_SPECIAL_RESPONSE) {
    return rc;
  }
  return NGX_DONE;
}
/* Request - */
