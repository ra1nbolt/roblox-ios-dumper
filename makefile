# --- environment settings ---
export THEOS = /home/sonic/theos
THEOS_DEVICE_IP = 10.0.1.27
THEOS_PACKAGE_SCHEME = roothide
TARGET = iphone:clang:latest:latest
INSTALL_TARGET_PROCESSES = Roblox

include $(THEOS)/makefiles/common.mk

# --- names & paths ---
TWEAK_NAME = RobloxDumper
CAP_DIR    = lib/capstone
CAP_A64    = $(CAP_DIR)/arch/AArch64
FMT_DIR    = lib/fmt
JSON_DIR   = lib/json

# --- files to be compiled ---
RobloxDumper_FILES = $(shell find src -name "*.cpp" -o -name "*.mm") \
                     $(wildcard $(CAP_DIR)/*.c) \
                     $(shell find $(CAP_A64) -name "*.c")

# --- compile-related vars ---
# git metadata
GIT_HASH     = $(shell git rev-parse --short HEAD)
IS_DEV_BUILD = $(shell git describe --tags --exact-match > /dev/null 2>&1 && echo false || echo true)

# general flags
RobloxDumper_CFLAGS = -fobjc-arc \
                      -Wno-unused-function \
                      -I$(CAP_DIR)/include/ \
                      -I$(FMT_DIR)/include/ \
                      -I$(JSON_DIR)/ \
                      -DGIT_HASH=\"$(GIT_HASH)\" \
                      -DIS_DEVBUILD=$(IS_DEV_BUILD) \
                      -DFMT_HEADER_ONLY \
                      -DCAPSTONE_HAS_ARM64 \
                      -DCAPSTONE_HAS_AARCH64 \
                      -DCAPSTONE_USE_SYS_DYN_MEM \
                      -DCAPSTONE_DIET=no

# c++ flags
RobloxDumper_CCFLAGS = -std=c++20 -x objective-c++

# obj-c++ flags
RobloxDumper_OBJCCFLAGS = -fno-modules -fno-autolink

# linker flags
RobloxDumper_LDFLAGS = -Xlinker -w

include $(THEOS_MAKE_PATH)/tweak.mk