/*
 * Copyright (c) 2026, Qualcomm Technologies, Inc. and/or its subsidiaries.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef CHIPINFO_IMAGE_H
#define CHIPINFO_IMAGE_H

#include "chipinfo_defs.h"
#include "chipinfo_local.h"
#include "platforminfo_defs.h"

/*
 * Common way to access the socinfo SMEM region,
 * since not all images have an SMEM driver for accessing socinfo.
 */
platforminfo_smem_t *chipinfo_get_soc_info(void);

/*
 * Common way to dynamically allocate memory on all images, since
 * different images might use different memory allocation functions.
 */
void *chipinfo_malloc(uint32_t size);

/*
 * Common way to copy memory on all images, since not all images
 * use the same safe-memcpy function.
 *
 * This is a wrapper around the image's safe memcpy, and doesn't
 * provide any additional functionality other than some minor
 * input validation.
 *
 */
void *chipinfo_memcpy(void *restrict dest, uint32_t dest_len,
		       const void *restrict src, uint32_t src_len);

/*
 * Common way to copy strings on all images, since different images
 * use different versions of a safe string copy. This function is a
 * wrapper around the image's safe string copy, and doesn't
 * implement anything extra other than some minor input validation.
 */
char *chipinfo_strlcpy(char *restrict dest, const char *restrict src,
		       uint32_t dest_len);

#endif /* CHIPINFO_IMAGE_H */
