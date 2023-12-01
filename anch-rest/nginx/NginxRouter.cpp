#include "rest/constants.hpp"

#include <dlfcn.h>

void
initializeModule() {
  void* handle = ::dlopen("", RTLD_NOW | RTLD_GLOBAL);
  if(handle == NULL) {
    throw -1;
  }
}

extern "C"
ngx_int_t
anch_rest_initialize_module() {
  try {
    initializeModule();
  } catch(...) {
    return -1;
  }
}

void
handleRequest(ngx_http_request_t* request) {
}

extern "C"
ngx_int_t
anch_rest_handle_request(ngx_http_request_t* request) {
  try {
    handleRequest(request);
  } catch(const anch::rest::StatusCode& code) {
    return code;
  } catch(...) {
    return 500;
  }
}
