# Component makefile for colour_conversion


INC_DIRS += $(colour_conversion_ROOT)


colour_conversion_INC_DIR = $(colour_conversion_ROOT)
colour_conversion_SRC_DIR = $(colour_conversion_ROOT)

$(eval $(call component_compile_rules,colour_conversion))
