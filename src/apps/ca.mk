teeclient_sub_dir:=$(cur_dir)/tee_client
SRC_DIRS+=$(teeclient_sub_dir)
INC_DIRS+=$(teeclient_sub_dir)/inc
-include $(teeclient_sub_dir)/sources.mk
C_SRC+=$(addprefix $(teeclient_sub_dir)/, $(gpapi_c_srcs))
ASM_SRC+=$(addprefix $(teeclient_sub_dir)/, $(gpapi_s_srcs))

ifeq ($(CONFIG_APPS_BW),y)
bw_sub_dir:=$(cur_dir)/../bw
SRC_DIRS+=$(bw_sub_dir)
INC_DIRS+=$(bw_sub_dir)/ca/include
-include $(bw_sub_dir)/sources.mk
C_SRC+=$(addprefix $(bw_sub_dir)/, $(ca_c_srcs))
ASM_SRC+=$(addprefix $(bw_sub_dir)/, $(ca_s_srcs))
CFLAGS += -DCONFIG_APPS_BW
endif

ifeq ($(CONFIG_APPS_HELLO_WORLD),y)
bw_sub_dir:=$(cur_dir)/../hello_world
SRC_DIRS+=$(bw_sub_dir)
INC_DIRS+=$(bw_sub_dir)
-include $(bw_sub_dir)/sources.mk
C_SRC+=$(addprefix $(bw_sub_dir)/, $(ca_c_srcs))
ASM_SRC+=$(addprefix $(bw_sub_dir)/, $(ca_s_srcs))
CFLAGS += -DCONFIG_APPS_HELLO_WORLD
endif

