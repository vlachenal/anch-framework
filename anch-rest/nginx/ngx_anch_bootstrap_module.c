#include "NginxRouter.hpp"

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

// Module declaration +
/*!
 * AnCH REST handler
 *
 * \param request the HTTP request
 *
 * \return done or declined
 */
static ngx_int_t ngx_anch_rest_handler(ngx_http_request_t* request);

void* application;

/*!
 * AnCH nginx bootstrap configuration structur
 */
typedef struct {
  ngx_flag_t enable;
  ngx_str_t app_lib_path;
  //void* application;
} ngx_anch_bootstrap_conf_t;

/*!
 * AnCH nginx bootstrap commands
 */
static ngx_command_t ngx_anch_bootstrap_commands[] = {
  {
    ngx_string("anch_bootstrap"),
    NGX_HTTP_MAIN_CONF | NGX_HTTP_SRV_CONF | NGX_HTTP_LOC_CONF | NGX_CONF_FLAG,
    ngx_conf_set_flag_slot,
    NGX_HTTP_LOC_CONF_OFFSET,
    offsetof(ngx_anch_bootstrap_conf_t, enable),
    NULL
  },
  ngx_null_command
};

/*!
 * AnCH nginx bootstrap initialization
 * \todo load library ...
 *
 * \param cf the configuration
 *
 * \return OK ...
 */
static ngx_int_t
ngx_anch_bootstrap_init(ngx_conf_t* cf) {
  ngx_http_next_request_body_filter = ngx_http_top_request_body_filter;
  ngx_http_top_request_body_filter = ngx_anch_rest_handler;
  return NGX_OK;
}

/*!
 * Create AnCH nginx bootstrap configuration
 *
 * \param cf the nginx configuration
 *
 * \return the module configuration
 */
static void*
ngx_anch_bootstrap_create_conf(ngx_conf_t* cf) {
  ngx_anch_bootstrap_conf_t* conf;
  conf = ngx_pcalloc(cf->pool, sizeof(ngx_anch_bootstrap_conf_t));
  if(conf == NULL) {
    return NULL;
  }
  conf->enable = NGX_CONF_UNSET;
  return conf;
}

/*!
 * Merge AnCH nginx bootstrap configuration ?
 *
 * \param cf the configuration
 * \param parent the parent ?
 * \param child the child ?
 *
 * \return status
 */
static char*
ngx_anch_bootstrap_merge_conf(ngx_conf_t* cf, void* parent, void* child) {
  ngx_anch_bootstrap_conf_t* prev = parent;
  ngx_anch_bootstrap_conf_t* conf = child;
  ngx_conf_merge_value(conf->enable, prev->enable, 0);
  return NGX_CONF_OK;
}

/*!
 * AnCH nginx bootstrap module context
 */
static ngx_http_module_t ngx_anch_bootstrap_module_ctx = {
  NULL,                           /* preconfiguration */
  ngx_anch_bootstrap_init,        /* postconfiguration */
  NULL,                           /* create main configuration */
  NULL,                           /* init main configuration */
  NULL,                           /* create server configuration */
  NULL,                           /* merge server configuration */
  ngx_anch_bootstrap_create_conf, /* create location configuration */
  ngx_anch_bootstrap_merge_conf   /* merge location configuration */
};

extern "C" {
  ngx_module_t  ngx_anch_bootstrap_module = {
    NGX_MODULE_V1,
    &ngx_anch_bootstrap_module_ctx, /* module context */
    ngx_anch_bootstrap_commands,    /* module directives */
    NGX_HTTP_MODULE,                /* module type */
    NULL,                           /* init master */
    NULL,                           /* init module */
    NULL,                           /* init process */
    NULL,                           /* init thread */
    NULL,                           /* exit thread */
    NULL,                           /* exit process */
    NULL,                           /* exit master */
    NGX_MODULE_V1_PADDING
  };
}
// Module declaration -

static ngx_int_t
ngx_anch_rest_handler(ngx_http_request_t* request) {
  ngx_int_t rc;
  ngx_anch_bootstrap_conf_t* dlcf;
  dlcf = ngx_http_get_module_loc_conf(request, ngx_anch_bootstrap_module);
  /*if(!(request->method & dlcf->methods)) {
    return NGX_DECLINED;
    }*/
  switch(request->method) {
  case NGX_HTTP_GET:
    break;
  case NGX_HTTP_POST:
    break;
  case NGX_HTTP_PATCH:
    break;
  case NGX_HTTP_PUT:
    break;
  case NGX_HTTP_DELETE:
    break;
  case NGX_HTTP_MKCOL:
    break;
  case NGX_HTTP_COPY:
    break;
  case NGX_HTTP_MOVE:
    break;
  default:
    return NGX_DECLINED;
  }

  rc = ngx_http_read_client_request_body(request, ngx_http_anch_rest_req_handler);
  return NGX_DONE;
}
