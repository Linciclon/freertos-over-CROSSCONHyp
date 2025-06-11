teeclient_sub_dir:=$(cur_dir)/tee_client
SRC_DIRS+=$(teeclient_sub_dir)
INC_DIRS+=$(teeclient_sub_dir)/inc
-include $(teeclient_sub_dir)/sources.mk
C_SRC+=$(addprefix $(teeclient_sub_dir)/, $(gpapi_c_srcs))
ASM_SRC+=$(addprefix $(teeclient_sub_dir)/, $(gpapi_s_srcs))


bw_sub_dir:=$(cur_dir)/../bw
SRC_DIRS+=$(bw_sub_dir)
INC_DIRS+=$(bw_sub_dir)/ca/include
-include $(bw_sub_dir)/sources.mk
C_SRC+=$(addprefix $(bw_sub_dir)/, $(ca_c_srcs))
ASM_SRC+=$(addprefix $(bw_sub_dir)/, $(ca_s_srcs))


