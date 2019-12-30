# Component makefile for rgbw_lights


INC_DIRS += $(rgbw_lights_ROOT)


rgbw_lights_INC_DIR = $(rgbw_lights_ROOT)
rgbw_lights_SRC_DIR = $(rgbw_lights_ROOT)

$(eval $(call component_compile_rules,rgbw_lights))
