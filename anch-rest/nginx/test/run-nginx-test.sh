#!/bin/sh
# AnCH REST nginx module integration test: build nginx with the module, serve the test application
# (anch-rest-nginx-test-app) and check responses with curl.
#
# Usage: run-nginx-test.sh <nginx sources directory> <AnCH build directory> [work directory]
#   nginx sources: extracted nginx release (https://nginx.org/download/)
#   AnCH build: build directory with anch-rest-nginx-test-app built (ANCH_BUILD_REST=ON)
set -eu

nginx_src=$(realpath "$1")
anch_build=$(realpath "$2")
work=$(realpath -m "${3:-${anch_build}/nginx-test}")
module_dir=$(realpath "$(dirname "$0")/..")
app="${anch_build}/lib/libanch-rest-nginx-test-app.so"
port=18080
sync_port=18081
failures=0

[ -f "${app}" ] || { echo "Test application not found: ${app}"; exit 1; }

# Build nginx with the module +
echo "== build nginx $(basename "${nginx_src}")"
cd "${nginx_src}"
if [ ! -f objs/ngx_http_anch_rest_module.so ] || [ "${module_dir}/ngx_http_anch_rest_module.c" -nt objs/ngx_http_anch_rest_module.so ]; then
  ./configure --with-threads --with-compat --without-http_rewrite_module --without-http_gzip_module \
    --add-dynamic-module="${module_dir}" > "${work}.configure.log" 2>&1 || { tail -20 "${work}.configure.log"; exit 1; }
  make -j"$(nproc)" > "${work}.make.log" 2>&1 || { grep -E 'error|warning' "${work}.make.log" | head -20; exit 1; }
fi
# Build nginx with the module -

# Configuration +
rm -rf "${work}"
mkdir -p "${work}/logs" "${work}/tmp" "${work}/www/static"
printf 'static' > "${work}/www/static/index.html"
cat > "${work}/nginx.conf" <<CONF
worker_processes 1;
error_log ${work}/logs/error.log info;
pid ${work}/nginx.pid;
load_module ${nginx_src}/objs/ngx_http_anch_rest_module.so;
thread_pool default threads=4;
events { worker_connections 64; }
http {
  access_log off;
  client_body_temp_path ${work}/tmp/body;
  proxy_temp_path ${work}/tmp/proxy;
  fastcgi_temp_path ${work}/tmp/fastcgi;
  uwsgi_temp_path ${work}/tmp/uwsgi;
  scgi_temp_path ${work}/tmp/scgi;
  client_body_buffer_size 1k;
  server {
    listen 127.0.0.1:${port};
    location /api/ { anch_rest_app ${app} "nginx configuration"; }
    location /fail/ { anch_rest_app ${app} fail; }
    location /static/ { root ${work}/www; }
  }
  server {
    listen 127.0.0.1:${sync_port};
    anch_rest_thread_pool off;
    location /api/ { anch_rest_app ${app} "nginx configuration"; }
  }
}
CONF
# Configuration -

# Checks +
check() {
  # check <description> <expected> <actual>
  if [ "$2" = "$3" ]; then
    echo "OK   $1"
  else
    echo "FAIL $1: expected '$2', found '$3'"
    failures=$((failures + 1))
  fi
}
url="http://127.0.0.1:${port}"
# Checks -

echo "== nginx -t"
LD_LIBRARY_PATH="${anch_build}/lib" "${nginx_src}/objs/nginx" -t -p "${work}" -c "${work}/nginx.conf" 2>&1 | tail -1
echo "== start nginx"
LD_LIBRARY_PATH="${anch_build}/lib" "${nginx_src}/objs/nginx" -p "${work}" -c "${work}/nginx.conf"
trap 'kill -QUIT "$(cat "${work}/nginx.pid")" 2> /dev/null || true' EXIT
sleep 1

check "GET json" '200 {"id":1,"name":"Rex"}' "$(curl -s -w '%{http_code} ' -o /tmp/body.$$ "${url}/api/pets/1"; cat /tmp/body.$$)"
check "GET content type" 'application/json' "$(curl -s -o /dev/null -w '%{content_type}' "${url}/api/pets/1")"
check "GET 404" '404' "$(curl -s -o /dev/null -w '%{http_code}' "${url}/api/pets/2")"
check "GET 406" '406' "$(curl -s -o /dev/null -w '%{http_code}' -H 'Accept: text/html' "${url}/api/pets/1")"
check "POST json" '201 /api/pets/7 Médor' "$(curl -s -D /tmp/headers.$$ -o /tmp/body.$$ -w '%{http_code}' -H 'Content-Type: application/json' -d '{"id":7,"name":"Médor"}' "${url}/api/pets"; printf ' %s %s' "$(grep -i '^Location:' /tmp/headers.$$ | tr -d '\r' | cut -d' ' -f2)" "$(cat /tmp/body.$$)")"
big=$(head -c 100000 /dev/zero | tr '\0' 'a')
check "POST body in temporary file" '201 100000' "$(printf '{"id":8,"name":"%s"}' "${big}" | curl -s -o /tmp/body.$$ -w '%{http_code}' -H 'Content-Type: application/json' --data-binary @- "${url}/api/pets"; printf ' %s' "$(wc -c < /tmp/body.$$ | tr -d ' ')")"
check "POST without body" '400' "$(curl -s -o /dev/null -w '%{http_code}' -X POST -H 'Content-Type: application/json' "${url}/api/pets")"
check "headers and query" 'value a b' "$(curl -s -H 'X-Test: value' "${url}/api/headers?q=a%20b")"
check "configuration" 'nginx configuration' "$(curl -s "${url}/api/config")"
check "HEAD" '200 0' "$(curl -s -I -o /dev/null -w '%{http_code} %{size_download}' -H 'Accept: application/json' "${url}/api/pets/1")"
check "405" '405 GET, HEAD, OPTIONS' "$(curl -s -D /tmp/headers.$$ -o /dev/null -w '%{http_code}' -X DELETE "${url}/api/pets/1"; printf ' %s' "$(grep -i '^Allow:' /tmp/headers.$$ | tr -d '\r' | cut -d' ' -f2-)")"
check "OPTIONS" '204 GET, HEAD, OPTIONS' "$(curl -s -D /tmp/headers.$$ -o /dev/null -w '%{http_code}' -X OPTIONS "${url}/api/pets/1"; printf ' %s' "$(grep -i '^Allow:' /tmp/headers.$$ | tr -d '\r' | cut -d' ' -f2-)")"
check "CORS preflight" '204 https://app.example.com POST, OPTIONS' "$(curl -s -D /tmp/headers.$$ -o /dev/null -w '%{http_code}' -X OPTIONS -H 'Origin: https://app.example.com' -H 'Access-Control-Request-Method: POST' -H 'Access-Control-Request-Headers: content-type' "${url}/api/pets"; printf ' %s %s' "$(grep -i '^Access-Control-Allow-Origin:' /tmp/headers.$$ | tr -d '\r' | cut -d' ' -f2-)" "$(grep -i '^Access-Control-Allow-Methods:' /tmp/headers.$$ | tr -d '\r' | cut -d' ' -f2-)")"
check "CORS preflight refused" '403' "$(curl -s -o /dev/null -w '%{http_code}' -X OPTIONS -H 'Origin: https://evil.example.com' -H 'Access-Control-Request-Method: POST' "${url}/api/pets")"
check "CORS GET" 'https://app.example.com' "$(curl -s -D /tmp/headers.$$ -o /dev/null -H 'Origin: https://app.example.com' "${url}/api/pets/1"; grep -i '^Access-Control-Allow-Origin:' /tmp/headers.$$ | tr -d '\r' | cut -d' ' -f2-)"
head -c 300000 /dev/urandom > "${work}/photo.png"
check "multipart upload (curl -F, body in temporary file)" '200 1 Rex photo.png image/png 300000' "$(curl -s -o /tmp/body.$$ -w '%{http_code}' -F 'description=Rex' -F "photo=@${work}/photo.png;type=image/png" "${url}/api/pets/1/photo"; printf ' %s' "$(cat /tmp/body.$$)")"
head -c 1100000 /dev/urandom > "${work}/big.png"
check "multipart over client_max_body_size" '413' "$(curl -s -o /dev/null -w '%{http_code}' -F "photo=@${work}/big.png" "${url}/api/pets/1/photo")"
check "exception" '500' "$(curl -s -o /dev/null -w '%{http_code}' "${url}/api/crash")"
check "application creation failure" '500' "$(curl -s -o /dev/null -w '%{http_code}' "${url}/fail/pets/1")"
check "other locations" 'static' "$(curl -s "${url}/static/")"

# Two slow requests (1 s each) on one worker: in parallel with thread pool, serialized without
elapsed() {
  start=$(date +%s%N)
  curl -s -o /dev/null "$1/api/slow" & curl -s -o /dev/null "$1/api/slow" & wait
  echo $(( ($(date +%s%N) - start) / 1000000 ))
}
threaded=$(elapsed "${url}")
blocking=$(elapsed "http://127.0.0.1:${sync_port}")
check "thread pool: parallel slow requests < 1.8 s (${threaded} ms)" 'yes' "$([ "${threaded}" -lt 1800 ] && echo yes || echo no)"
check "thread pool off: serialized slow requests >= 1.9 s (${blocking} ms)" 'yes' "$([ "${blocking}" -ge 1900 ] && echo yes || echo no)"

kill -QUIT "$(cat "${work}/nginx.pid")"
trap - EXIT
sleep 1
check "creation failure logged" '1' "$(grep -c 'creation failed: configured to fail' "${work}/logs/error.log")"
check "no other alert in error log" '0' "$(grep -E '\[(alert|crit|emerg)\]' "${work}/logs/error.log" | grep -vc 'creation failed: configured to fail')"
rm -f /tmp/body.$$ /tmp/headers.$$

echo "== ${failures} failure(s)"
[ "${failures}" -eq 0 ]
