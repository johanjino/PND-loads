add_llvm_library( LLVMAliasHints MODULE
		AliasHints.cpp

		PLUGIN_TOOL
		opt
)
add_dependencies(LLVMAliasHints intrinsics_gen)
add_llvm_pass_plugin(AliasHintsPass AliasHints.cpp)
