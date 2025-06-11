# Prepare enviroment for baremetal-runtime
NAME:=freertos
ROOT_DIR:=$(realpath .)
BUILD_DIR:=$(ROOT_DIR)/build/$(PLATFORM)

# Setup baremetal-runtime build
bmrt_dir:=$(ROOT_DIR)/src/baremetal-runtime
include $(bmrt_dir)/setup.mk

$(info === Setup baremetal-runtime build ===)

# Inlcude all freertos sources

$(info === Main freertos application sources ===)

# Main freertos application sources
APP_SRC_DIR?=$(ROOT_DIR)/src/freertos-app
include $(APP_SRC_DIR)/sources.mk
C_SRC+=$(addprefix $(APP_SRC_DIR)/, $(src_c_srcs))

$(info === Freertos kernel sources ===)

# Freertos kernel sources
freertos_dir:=$(ROOT_DIR)/src/freertos
SRC_DIRS+=$(freertos_dir) $(freertos_memmng_dir)
INC_DIRS+=$(freertos_dir)/include
C_SRC+=$(wildcard $(freertos_dir)/*.c)
freertos_memmng_dir:=$(freertos_dir)/portable/MemMang
C_SRC+=$(freertos_memmng_dir)/heap_4.c

$(info === Freertos port arch-specific ===)

# Freertos port arch-specific
arch_dir=$(ROOT_DIR)/src/arch/$(ARCH)
SRC_DIRS+=$(arch_dir)
INC_DIRS+=$(arch_dir)/inc
-include $(arch_dir)/arch.mk
-include $(arch_dir)/sources.mk
C_SRC+=$(addprefix $(arch_dir)/, $(arch_c_srcs))
ASM_SRC+=$(addprefix $(arch_dir)/, $(arch_s_srcs))

$(info === Freertos port Extra Files ===)

include $(APP_SRC_DIR)/extra.mk

$(info === GP Client API Files ===)

GPClientAPI_SRC_DIR?=$(ROOT_DIR)/src/apps
include $(GPClientAPI_SRC_DIR)/ca.mk

$(info === Client Applications (BW for the moment) ===)

# ClientAPP_SRC_DIR?=$(ROOT_DIR)/src/apps/bw
# SRC_DIRS+=$(ClientAPP_SRC_DIR)
# INC_DIRS+=$(ClientAPP_SRC_DIR)/ca/include
# include $(ClientAPP_SRC_DIR)/sources.mk
# C_SRC+=$(addprefix $(ClientAPP_SRC_DIR)/, $(ca_c_srcs))
# ASM_SRC+=$(addprefix $(ClientAPP_SRC_DIR)/, $(ca_c_srcs))

$(info FOO is: $(INC_DIRS))

$(info === Include the final baremetal-runtime build ===)

# Include the final baremetal-runtime build
include $(bmrt_dir)/build.mk
