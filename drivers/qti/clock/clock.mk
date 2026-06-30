#
# Copyright (c) 2026 Qualcomm Technologies, Inc. and/or its subsidiaries.
#
# SPDX-License-Identifier: BSD-3-Clause
#
# Clock driver
#

$(eval $(call add_define,QTI_CLOCK_ENABLED))

CLOCK_DRV_PATH := drivers/qti/clock

PLAT_INCLUDES += \
	-Iinclude/drivers/qti/clock

BL31_SOURCES += \
	$(CLOCK_DRV_PATH)/clock_driver.c			\
	$(CLOCK_DRV_PATH)/clock_hal.c				\
	$(CLOCK_DRV_PATH)/$(CHIPSET)/clock_tfa.c		\
	$(CLOCK_DRV_PATH)/$(CHIPSET)/clock_bsp.c

# RPM-based chipsets have no BCM; gate RPMh/voltage code in clock_driver.c.
# Add RPM-based chipset names to this list when bringing them up.
CLOCK_RPM_CHIPSETS :=
ifneq ($(filter $(CHIPSET),$(CLOCK_RPM_CHIPSETS)),)
$(eval $(call add_define,QTI_CLOCK_NO_BCM))
endif

# The clock driver votes for NoC bandwidth during clock-group bring-up, so the
# platform must also build the ICB micro-arbiter (drivers/qti/icb/uarb), which
# provides icbuarb_create_client() / icbuarb_issue_request().
