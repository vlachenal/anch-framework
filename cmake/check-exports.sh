#!/bin/sh
# Check the symbols exported by AnCH shared libraries.
#
# Usage: cmake/check-exports.sh <libraries directory>
#
# AnCH libraries are built with hidden symbols visibility (see anch_add_library in cmake/AnchHelpers.cmake).
# This script fails when:
# - a library exports a symbol outside the anch namespace which is not an expected entry point (SQL drivers
#   'create_connection' functions loaded with dlsym, nginx application, anch-ut main). Instantiations of the C++
#   standard library and of third party libraries templates (default visibility) are ignored;
# - a library does not export any anch symbol (hidden visibility without export macro).
#
# Symbols are classified with their mangled names (Itanium C++ ABI): demangled names of templates are not reliable
# (return types such as 'decltype(...)', constrained templates which c++filt can not demangle).
set -eu

[ $# -eq 1 ] || { echo "Usage: $0 <libraries directory>"; exit 1; }
libdir=$1

status=0
found=0
for lib in "${libdir}"/*.so; do
  [ -e "${lib}" ] || continue
  found=1
  name=$(basename "${lib}")
  report=$(nm -D --defined-only "${lib}" | awk '
    {
      sym = $3
      # Entry points declared outside the anch namespace
      if(sym ~ /^(create_connection|create_shared_connection|delete_connection|anch_rest_application|main)$/) next
      # AddressSanitizer ODR indicators
      if(sym ~ /^__odr_asan\./) next
      # Global placement new and delete (inline functions emitted without optimization)
      if(sym ~ /^_Z(nw|na|dl|da)/) next
      # Type information of builtin, pointer and function types (typeinfo for void (*)(), ...)
      if(sym ~ /^_ZT[IS][PKRFA]*[a-zF]/) next
      # Special names (typeinfo, vtable, guard variable, thunks, ...) and nested names prefixes (H: explicit object
      # parameter, K, V: const and volatile, R, O: reference qualifiers)
      rest = sym
      sub(/^_Z(T[a-zA-Z]|G[VR]|Th[n]?[0-9]+_|Tv[n]?[0-9]+_n?[0-9]+_)?Z?N?[HKVRO]*/, "", rest)
      # anch namespace
      if(rest ~ /^4anch/) { anch = 1; next }
      # Standard library (St and its substitutions) and third party namespaces
      if(rest ~ /^(St|S[absiod]|9__gnu_cxx|10__cxxabiv1|4absl|6google|4grpc|9grpc_core|6apache|5boost)/) next
      unexpected[sym] = 1
    }
    END {
      if(!anch) print "-"
      for(sym in unexpected) print sym
    }')
  if printf '%s\n' "${report}" | grep -qx -- '-'; then
    echo "${name}: no anch symbol is exported"
    status=1
  fi
  unexpected=$(printf '%s\n' "${report}" | grep -vx -- '-' | grep -v '^$' || true)
  if [ -n "${unexpected}" ]; then
    echo "${name}: unexpected exported symbols:"
    printf '%s\n' "${unexpected}" | c++filt | sort | sed 's/^/  /'
    status=1
  fi
done

[ "${found}" -eq 1 ] || { echo "No shared library in ${libdir}"; exit 1; }
[ "${status}" -eq 0 ] && echo "Exported symbols: OK"
exit "${status}"
