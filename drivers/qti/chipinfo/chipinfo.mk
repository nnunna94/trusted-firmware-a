#
# Copyright (c) 2026, Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause
#

CHIPINFO_SOURCES	:=	drivers/qti/chipinfo/chipinfo.c		\
				drivers/qti/chipinfo/chipinfo_core.c	\
				drivers/qti/chipinfo/chipinfo_os.c

# Add include path for chipinfo headers
PLAT_INCLUDES		+=	-Iinclude/drivers/qti/chipinfo
