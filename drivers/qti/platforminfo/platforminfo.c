/*
 * Copyright (c) 2026, Qualcomm Technologies, Inc. and/or its subsidiaries.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>
#include <string.h>

#include "platforminfo.h"
#include "smem.h"

typedef struct {
	platforminfo_platform_type_t platform;
	uint32_t version;
	uint32_t subtype;
	uint32_t oem_variant_id;
} platforminfo_platform_info_t;

typedef struct {
	uint32_t key;
	uint32_t value;
} platforminfo_kvps_t;

typedef struct platforminfo_drv_ctxt {
	platforminfo_platform_info_t platform_info;
	uint32_t num_kvps;
	platforminfo_kvps_t kvps_static[PLATFORMINFO_MAX_KVPS];
} platforminfo_drv_ctxt_t;

static platforminfo_drv_ctxt_t platforminfo_driver_ctxt;

platforminfo_result_t platforminfo_init(void)
{
	platforminfo_drv_ctxt_t *drv_ctxt = &platforminfo_driver_ctxt;
	uint32_t smem_size;
	uint32_t kvps_size;
	platforminfo_smem_t *smem = NULL;
	platforminfo_kvps_t *kvps_data = NULL;

	if (drv_ctxt->platform_info.platform != PLATFORMINFO_TYPE_UNKNOWN) {
		return PLATFORMINFO_SUCCESS;
	}

	smem = (platforminfo_smem_t *)smem_get_addr(SMEM_HW_SW_BUILD_ID,
							&smem_size);
	if (smem == NULL || smem_size == 0) {
		return PLATFORMINFO_ERROR_NOT_FOUND;
	}

	drv_ctxt->platform_info.platform = smem->platform_type;
	drv_ctxt->platform_info.version = smem->platform_version;
	drv_ctxt->platform_info.subtype = smem->platform_subtype;

	/* OEM Variant ID support added from SMEM Structure formtat version 17. */
	if (smem->format >= PLATFORMINFO_SMEM_FORMAT_VERSION(17)) {
		drv_ctxt->platform_info.oem_variant_id = smem->oem_variant_id;
	} else {
		drv_ctxt->platform_info.oem_variant_id = 0;
	}

    /* KVP support added from SMEM Structure formtat version 17. */
	if (smem->format >= PLATFORMINFO_SMEM_FORMAT_VERSION(18)) {
		drv_ctxt->num_kvps = smem->num_kvps;

		if (drv_ctxt->num_kvps > 0) {
			if (drv_ctxt->num_kvps > PLATFORMINFO_MAX_KVPS) {
				drv_ctxt->num_kvps = PLATFORMINFO_MAX_KVPS;
			}

			kvps_size = sizeof(platforminfo_kvps_t) *
					drv_ctxt->num_kvps;

			if (smem->kvps_offset > smem_size ||
			    kvps_size > smem_size - smem->kvps_offset) {
				return PLATFORMINFO_ERROR_INVALID_DATA;
			}

			kvps_data = (platforminfo_kvps_t *)((uintptr_t)smem +
								smem->kvps_offset);

			(void)memcpy(drv_ctxt->kvps_static, kvps_data,
				     kvps_size);
		}
	}

	return PLATFORMINFO_SUCCESS;
}

void platforminfo_driver_init(void)
{
	platforminfo_result_t ret;

	ret = platforminfo_init();
	assert(ret == PLATFORMINFO_SUCCESS);
}

platforminfo_platform_type_t platforminfo_get_platform(void)
{
	return platforminfo_driver_ctxt.platform_info.platform;
}

uint32_t platforminfo_get_version(void)
{
	return platforminfo_driver_ctxt.platform_info.version;
}

uint32_t platforminfo_get_subtype(void)
{
	return platforminfo_driver_ctxt.platform_info.subtype;
}

platforminfo_result_t platforminfo_get_key_value(platforminfo_key_type_t key,
						uint32_t *value)
{
	platforminfo_drv_ctxt_t *drv_ctxt = &platforminfo_driver_ctxt;
	uint32_t i;

	if (value == NULL) {
		return PLATFORMINFO_ERROR_INVALID_PARAMETER;
	}

	for (i = 0; i < drv_ctxt->num_kvps; i++) {
		if (key == drv_ctxt->kvps_static[i].key) {
			*value = drv_ctxt->kvps_static[i].value;
			return PLATFORMINFO_SUCCESS;
		}
	}

	return PLATFORMINFO_ERROR_NOT_FOUND;
}

uint32_t platforminfo_get_oem_variant_id(void)
{
	return platforminfo_driver_ctxt.platform_info.oem_variant_id;
}
