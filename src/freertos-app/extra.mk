ARCH_GENERIC_FLAGS += -DGUEST

extra_cmsis_sub_dir:=$(cur_dir)/CMSIS
SRC_DIRS+=$(extra_cmsis_sub_dir)
INC_DIRS+=$(extra_cmsis_sub_dir)/inc
-include $(extra_cmsis_sub_dir)/sources.mk
C_SRC+=$(addprefix $(extra_cmsis_sub_dir)/, $(sub_extra_c_srcs))
ASM_SRC+=$(addprefix $(extra_cmsis_sub_dir)/, $(sub_extra_s_srcs))

extra_device_sub_dir:=$(cur_dir)/../device
SRC_DIRS+=$(extra_device_sub_dir)
INC_DIRS+=$(extra_device_sub_dir)/inc
-include $(extra_device_sub_dir)/sources.mk
C_SRC+=$(addprefix $(extra_device_sub_dir)/, $(sub_extra_c_srcs))
ASM_SRC+=$(addprefix $(extra_device_sub_dir)/, $(sub_extra_s_srcs))


extra_drivers_sub_dir:=$(cur_dir)/../drivers
SRC_DIRS+=$(extra_drivers_sub_dir)
INC_DIRS+=$(extra_drivers_sub_dir)/inc
-include $(extra_drivers_sub_dir)/sources.mk
C_SRC+=$(addprefix $(extra_drivers_sub_dir)/, $(sub_extra_c_srcs))
ASM_SRC+=$(addprefix $(extra_drivers_sub_dir)/, $(sub_extra_s_srcs))


extra_board_sub_dir:=$(cur_dir)/../board
SRC_DIRS+=$(extra_board_sub_dir)
INC_DIRS+=$(extra_board_sub_dir)/inc
-include $(extra_board_sub_dir)/sources.mk
C_SRC+=$(addprefix $(extra_board_sub_dir)/, $(sub_extra_c_srcs))
ASM_SRC+=$(addprefix $(extra_board_sub_dir)/, $(sub_extra_s_srcs))








