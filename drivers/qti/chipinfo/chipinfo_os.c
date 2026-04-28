/*
 * Copyright (c) 2026, Qualcomm Technologies, Inc. and/or its subsidiaries.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <string.h>

#include "chipinfo_image.h"
#include "platforminfo_defs.h"
#include "smem.h"

platforminfo_smem_t *chipinfo_get_soc_info(void)
{
	platforminfo_smem_t *smem = NULL;
	uint32_t size;

	smem = (platforminfo_smem_t *)smem_get_addr(SMEM_HW_SW_BUILD_ID,
							&size);
	if (smem == NULL || size == 0) {
		return NULL;
	}

	return smem;
}

void *chipinfo_memcpy(void *restrict dest, uint32_t dest_len,
				const void *restrict src, uint32_t src_len)
{
	uint32_t copy_len;

	if (dest == NULL || dest_len == 0 || src == NULL || src_len == 0) {
		return NULL;
	}

	copy_len = (src_len < dest_len) ? src_len : dest_len;
	memcpy(dest, src, copy_len);

	return dest;
}

char *chipinfo_strlcpy(char *dest, const char *src, uint32_t dest_len)
{
	if (dest == NULL || src == NULL || dest_len == 0) {
		return NULL;
	}

	strlcpy(dest, src, dest_len);
	return dest;
}

