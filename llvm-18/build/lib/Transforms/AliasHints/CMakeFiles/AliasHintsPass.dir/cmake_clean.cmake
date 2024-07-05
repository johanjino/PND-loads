file(REMOVE_RECURSE
  "../../libAliasHintsPass.a"
  "../../libAliasHintsPass.pdb"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/AliasHintsPass.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
