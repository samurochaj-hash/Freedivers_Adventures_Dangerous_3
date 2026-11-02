# Requires devkitPro/devkitPPC with libogc installed.
include $(DEVKITPPC)/wii_rules

TARGET      := boot
BUILD       := build
SOURCES     := source
INCLUDES    :=
DATA        :=
LIBS        := -logc -lm
CFLAGS      = -g -O2 -Wall $(MACHDEP) $(INCLUDE)
CXXFLAGS    = $(CFLAGS)
LDFLAGS     = $(MACHDEP) -Wl,-Map,$(notdir $@).map

# The Wii template rules provide the standard ELF/DOL build targets.
include $(DEVKITPPC)/wii_rules
