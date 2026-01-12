dirs-y :=

dirs-$(CONFIG_USER_UTILS_TEST_IPC_SRV) += ipc_srv
dirs-$(CONFIG_USER_UTILS_TEST_IPC_CLIENT) += ipc_client

include $(patsubst %,$(MDIR)%/module.mk,$(dirs-y))

