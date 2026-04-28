#
# Copyright (c) 2026, Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause
#

PLATFORMINFO_SOURCES	:=	drivers/qti/platforminfo/platforminfo.c

# Add include path for platform_info headers
PLAT_INCLUDES		+=	-Iinclude/drivers/qti/platforminfo

BL31_SOURCES         += $(PLATFORMINFO_SOURCES)
