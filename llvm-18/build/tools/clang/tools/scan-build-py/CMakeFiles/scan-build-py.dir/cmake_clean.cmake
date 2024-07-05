file(REMOVE_RECURSE
  "../../../../bin/analyze-build"
  "../../../../bin/intercept-build"
  "../../../../bin/scan-build-py"
  "../../../../lib/libear/__init__.py"
  "../../../../lib/libear/config.h.in"
  "../../../../lib/libear/ear.c"
  "../../../../lib/libscanbuild/__init__.py"
  "../../../../lib/libscanbuild/analyze.py"
  "../../../../lib/libscanbuild/arguments.py"
  "../../../../lib/libscanbuild/clang.py"
  "../../../../lib/libscanbuild/compilation.py"
  "../../../../lib/libscanbuild/intercept.py"
  "../../../../lib/libscanbuild/report.py"
  "../../../../lib/libscanbuild/resources/scanview.css"
  "../../../../lib/libscanbuild/resources/selectable.js"
  "../../../../lib/libscanbuild/resources/sorttable.js"
  "../../../../lib/libscanbuild/shell.py"
  "../../../../libexec/analyze-c++"
  "../../../../libexec/analyze-cc"
  "../../../../libexec/intercept-c++"
  "../../../../libexec/intercept-cc"
  "CMakeFiles/scan-build-py"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/scan-build-py.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
