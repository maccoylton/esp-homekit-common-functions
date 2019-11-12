# Component makefile for shared_functions


INC_DIRS += $(shared_functions_ROOT)


shared_functions_INC_DIR = $(shared_functions_ROOT)
shared_functions_SRC_DIR = $(shared_functions_ROOT)

$(eval $(call component_compile_rules,shared_functions))
