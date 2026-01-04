export CC
export AS
export LD
export AR
export STRIP
export OBJCOPY
export HOST_CC
export HOST_AS
export HOST_LD
export HOST_AR
export HOST_STRIP
export HOST_OBJCOPY
export CROSS_COMPILE

HOST_CC      := $(CC)
HOST_AS      := $(AS)
HOST_LD      := $(LD)
HOST_AR      := $(AR)
HOST_STRIP   := $(STRIP)
HOST_OBJCOPY := $(OBJCOPY)

ifeq ($(CONFIG_BUILD_USE_CLANG),y)
    CC := clang
    AS := clang

    ifeq ($(CONFIG_BUILD_USE_WEVERYTHING),y)
        cflags-y += -Weverything                 \
                    -Wno-reserved-id-macro       \
                    -Wno-newline-eof             \
                    -Wno-padded                  \
                    -Wno-sign-conversion         \
                    -Wno-documentation           \
                    -Wno-cast-qual               \
                    -Wno-pedantic                \
                    -Wno-implicit-int-conversion \
                    -Wno-atomic-implicit-seq-cst \
                    -Wno-bad-function-cast       \
                    -Wno-cast-align              \
                    -Wno-packed                  \
                    -Wno-unknown-warning-option  \
                    -Wno-date-time               \
                    -Wno-reserved-identifier     \
                    -Wno-extra-semi-stmt
    endif
else
    ifneq ($(CROSS_COMPILE),)
        CC      := $(CROSS_COMPILE)gcc
        AS      := $(CROSS_COMPILE)gcc
        LD      := $(CROSS_COMPILE)ld
        AR      := $(CROSS_COMPILE)ar
        STRIP   := $(CROSS_COMPILE)strip
        OBJCOPY := $(CROSS_COMPILE)objcopy
    endif
endif

STRIP      = $(CROSS_COMPILE)strip

cflags-$(CONFIG_BUILD_USE_WERROR) += -Werror

BUILDSUBDIR = $(subst $(MAINDIR),$(BUILDDIR),$(MDIR))

mapout = $(foreach obj,$(1),$(subst $(MAINDIR),$(BUILDDIR),$(MDIR)$(obj)))

src2build = $(subst $(MAINDIR),$(BUILDDIR),$(1))
