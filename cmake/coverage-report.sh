#!/bin/sh
# Coverage report of a build made with the coverage preset (Clang source-based coverage).
#
# Usage: cmake/coverage-report.sh [build directory] [report directory]
#   cmake --preset coverage && cmake --build --preset coverage && ctest --preset coverage
#   cmake/coverage-report.sh build/coverage
#
# Outputs: <report directory>/summary.txt (per file and total), <report directory>/modules.md (lines per module,
# Markdown table) and <report directory>/html/index.html.
# Test sources, system headers and generated code (build directory) are not counted.
# LLVM_PROFDATA and LLVM_COV environment variables can select the LLVM tools (default: llvm-profdata, llvm-cov).
set -eu

build=$(realpath "${1:-build/coverage}")
report=$(realpath -m "${2:-${build}/coverage-report}")
profdata=${LLVM_PROFDATA:-llvm-profdata}
cov=${LLVM_COV:-llvm-cov}
profiles="${build}/profiles"
ignore='(^/usr/|/test/|/build/)'

if ! ls "${profiles}"/*.profraw > /dev/null 2>&1; then
  echo "No profile in ${profiles}: run 'ctest --preset coverage' first"
  exit 1
fi

# Merge profiles of every test process +
"${profdata}" merge -sparse "${profiles}"/*.profraw -o "${build}/coverage.profdata"
# Merge profiles of every test process -

# Instrumented objects: libraries (real files, not symbolic links) and executables +
first=""
objects=""
for object in $(find "${build}/lib" "${build}/bin" -type f \( -name '*.so*' -o -perm -u+x \) | sort); do
  if [ -z "${first}" ]; then
    first="${object}"
  else
    objects="${objects} -object ${object}"
  fi
done
[ -n "${first}" ] || { echo "No instrumented object in ${build}/lib and ${build}/bin"; exit 1; }
# Instrumented objects: libraries (real files, not symbolic links) and executables -

mkdir -p "${report}"
# shellcheck disable=SC2086 # objects are split on purpose
"${cov}" report "${first}" ${objects} -instr-profile="${build}/coverage.profdata" \
  -ignore-filename-regex="${ignore}" > "${report}/summary.txt"
# shellcheck disable=SC2086
"${cov}" show "${first}" ${objects} -instr-profile="${build}/coverage.profdata" \
  -ignore-filename-regex="${ignore}" -format=html -show-line-counts-or-regions -output-dir="${report}/html" > /dev/null

# Lines per module (first directory of file paths, which are relative to sources directory) +
awk '
  $1 ~ /^anch-[^\/]+\// && NF >= 13 {
    module = substr($1, 1, index($1, "/") - 1)
    lines[module] += $8
    missed[module] += $9
  }
  $1 == "TOTAL" { total = $8 ; totalMissed = $9 }
  END {
    print "| Module | Lines | Covered |"
    print "|---|---:|---:|"
    for(module in lines) {
      printf "| %s | %d | %.1f %% |\n", module, lines[module], 100 * (lines[module] - missed[module]) / lines[module] | "sort"
    }
    close("sort")
    printf "| **Total** | %d | **%.1f %%** |\n", total, 100 * (total - totalMissed) / total
  }' "${report}/summary.txt" > "${report}/modules.md"
# Lines per module -

cat "${report}/modules.md"
echo "Report: ${report}/summary.txt, ${report}/html/index.html"
