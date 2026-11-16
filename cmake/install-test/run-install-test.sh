#!/bin/sh
# Install AnCH Framework and build a consumer project with find_package(Anch).
#
# Usage: cmake/install-test/run-install-test.sh <work directory> [CMake options for AnCH...]
#   cmake/install-test/run-install-test.sh /tmp/anch-install -DBUILD_SHARED_LIBS=OFF
# AnCH is configured and built in <work directory>/build (tests are not built), installed in
# <work directory>/prefix, the consumer is built in <work directory>/consumer and executed.
set -eu

[ $# -ge 1 ] || { echo "Usage: $0 <work directory> [CMake options...]"; exit 1; }
work=$(realpath -m "$1")
shift
src=$(realpath "$(dirname "$0")/../..")
consumer=$(realpath "$(dirname "$0")")

rm -rf "${work}"
echo "== build and install AnCH ($*)"
cmake -S "${src}" -B "${work}/build" -DCMAKE_BUILD_TYPE=Release -DANCH_BUILD_TESTS=OFF \
  -DCMAKE_INSTALL_PREFIX="${work}/prefix" "$@" > "${work}.configure.log" 2>&1 || { tail -20 "${work}.configure.log"; exit 1; }
cmake --build "${work}/build" --parallel "$(nproc)" > "${work}.build.log" 2>&1 || { grep -E 'error' "${work}.build.log" | head -20; exit 1; }
cmake --install "${work}/build" > "${work}.install.log" 2>&1 || { tail -20 "${work}.install.log"; exit 1; }
# Sources and build directory are removed: the consumer can only use the installed files
rm -rf "${work}/build"

echo "== build consumer"
cmake -S "${consumer}" -B "${work}/consumer" -DCMAKE_PREFIX_PATH="${work}/prefix" -DCMAKE_BUILD_TYPE=Release
cmake --build "${work}/consumer" --parallel "$(nproc)"

echo "== run consumer"
"${work}/consumer/anch-install-test"
