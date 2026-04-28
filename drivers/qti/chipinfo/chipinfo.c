/*
 * Copyright (c) 2026, Qualcomm Technologies, Inc. and/or its subsidiaries.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "chipinfo_image.h"
#include "chipinfo_local.h"

chipinfo_version_t chipinfo_get_chip_version(void)
{
	return chipinfo_get_ctxt()->version;
}

uint32_t chipinfo_get_raw_chip_version(void)
{
	return chipinfo_get_ctxt()->rev_number;
}

chipinfo_id_t chipinfo_get_chip_id(void)
{
	return chipinfo_get_ctxt()->chipinfo_id;
}

uint32_t chipinfo_get_raw_chip_id(void)
{
	return chipinfo_get_ctxt()->raw_part_num;
}

chipinfo_result_t chipinfo_get_chip_id_string(char *sz_chip_id_str,
						uint32_t n_max_len)
{
	uint32_t len = 0;

	if (n_max_len < CHIPINFO_MAX_ID_LENGTH) {
		len = n_max_len;
	} else {
		len = CHIPINFO_MAX_ID_LENGTH;
	}

	chipinfo_strlcpy(sz_chip_id_str, chipinfo_get_ctxt()->chip_id_string, len);
	return CHIPINFO_SUCCESS;
}

chipinfo_family_t chipinfo_get_chip_family(void)
{
	return chipinfo_get_ctxt()->family_id;
}

chipinfo_result_t
chipinfo_get_marketing_name_string(char *psz_marketing_name_str,
					uint32_t maxlen)
{
	chipinfo_get_chip_id_string(psz_marketing_name_str, maxlen);
	return CHIPINFO_SUCCESS;
}

chipinfo_modem_t chipinfo_get_modem_support(void)
{
	return chipinfo_get_ctxt()->modem_supported;
}

chipinfo_serial_num_t chipinfo_get_serial_number(void)
{
	return chipinfo_get_ctxt()->serial_num;
}

chipinfo_foundry_id_t chipinfo_get_foundry_id(void)
{
	return chipinfo_get_ctxt()->foundry_id;
}

uint32_t chipinfo_get_raw_device_family(void)
{
	return chipinfo_get_ctxt()->raw_family_id;
}

uint32_t chipinfo_get_raw_device_number(void)
{
	return chipinfo_get_ctxt()->family_device_id;
}

chipinfo_qfprom_chip_id_t chipinfo_get_qfprom_chip_id(void)
{
	return chipinfo_get_ctxt()->qfprom_chip_id;
}

chipinfo_result_t chipinfo_get_disabled_cpus(uint32_t cpu_cluster,
							uint32_t *mask)
{
	if (mask == NULL) {
		return CHIPINFO_ERROR;
	}

	if (cpu_cluster == 0 && !chipinfo_get_ctxt()->num_clusters) {
		/*
		 * If this target doesn't have a binning plan, we don't know
		 * how many CPU clusters there are for this target, and
		 * chip_info.num_clusters will be left at its default value
		 * of 0. This is the only case where num_clusters will be 0.
		 *
		 * Some clients call this function even if a binning plan isn't
		 * defined, because they have target-agnostic code. These
		 * clients expect it to return successfully instead of
		 * OUT_OF_RANGE for the first cluster (0) when PTE hasn't blown
		 * fuses, since there's always at least 1 core that's working
		 * (the one that's running this function).
		 *
		 * chipinfo_get_ctxt()->cpu_clusters will be NULL until it's
		 * initialized by the target-specific function. That function
		 * will only be defined if we have a binning plan, so it'll
		 * still be NULL if we get to this point. We therefore need an
		 * explicit case here, where we return successfully with
		 * pn_mask = "enabled/unknown", instead of just accessing
		 * cpu_clusters like the general case below.
		 */
		*mask = 0;
		return CHIPINFO_SUCCESS;
	}

	if (cpu_cluster >= chipinfo_get_ctxt()->num_clusters) {
		return CHIPINFO_ERROR_OUT_OF_RANGE_PARAMETER;
	}

	*mask = chipinfo_get_ctxt()->cpu_clusters[cpu_cluster];
	return CHIPINFO_SUCCESS;
}

chipinfo_result_t chipinfo_get_sku(chipinfo_sku_t *info)
{
	if (!info) {
		return CHIPINFO_ERROR_INVALID_PARAMETER;
	}

	info->feature_code = chipinfo_get_ctxt()->feature_code;
	info->p_code = chipinfo_get_ctxt()->p_code;

	if (chipinfo_get_ctxt()->feature_code == CHIPINFO_FEATURE_CODE_UNKNOWN &&
	    chipinfo_get_ctxt()->p_code == CHIPINFO_P_CODE_UNKNOWN) {
		return CHIPINFO_ERROR_NOT_SUPPORTED;
	}
	return CHIPINFO_SUCCESS;
}

chipinfo_result_t chipinfo_get_num_functional_clusters(uint32_t *num_clusters)
{
	if (!num_clusters) {
		return CHIPINFO_ERROR_INVALID_PARAMETER;
	}
	if (!chipinfo_get_ctxt()->init_complete) {
		return CHIPINFO_ERROR_NOT_INITIALIZED;
	}

	*num_clusters = chipinfo_get_ctxt()->num_functional_clusters;
	return CHIPINFO_SUCCESS;
}

chipinfo_result_t chipinfo_get_boot_cluster_and_core(uint32_t *cluster,
								uint32_t *core)
{
	if (!cluster || !core) {
		return CHIPINFO_ERROR_INVALID_PARAMETER;
	}
	if (!chipinfo_get_ctxt()->init_complete) {
		return CHIPINFO_ERROR_NOT_INITIALIZED;
	}

	*cluster = chipinfo_get_ctxt()->boot_cluster;
	*core = chipinfo_get_ctxt()->boot_core;
	return CHIPINFO_SUCCESS;
}

chipinfo_result_t chipinfo_get_disabled_features(chipinfo_part_t part,
							uint32_t idx, uint32_t *mask)
{
	if (mask == NULL || part == CHIPINFO_PART_UNKNOWN) {
		return CHIPINFO_ERROR_INVALID_PARAMETER;
	}

	if (part >= CHIPINFO_NUM_PARTS) {
		return CHIPINFO_ERROR_OUT_OF_RANGE_PARAMETER;
	}

	if (!chipinfo_get_ctxt()->init_complete) {
		return CHIPINFO_ERROR_NOT_INITIALIZED;
	}

	*mask = chipinfo_get_ctxt()->partial_features[part];
	return CHIPINFO_SUCCESS;
}

chipinfo_result_t chipinfo_is_part_disabled(chipinfo_part_t part,
							uint32_t idx, bool *mask)
{
	if (mask == NULL || part == CHIPINFO_PART_UNKNOWN) {
		return CHIPINFO_ERROR_INVALID_PARAMETER;
	}

	if (part >= CHIPINFO_NUM_PARTS) {
		return CHIPINFO_ERROR_OUT_OF_RANGE_PARAMETER;
	}

	if (!chipinfo_get_ctxt()->init_complete) {
		return CHIPINFO_ERROR_NOT_INITIALIZED;
	}

	*mask = chipinfo_get_ctxt()->disabled_features[part] & 0x1 ? true : false;
	return CHIPINFO_SUCCESS;
}

uint32_t chipinfo_get_raw_package_type(void)
{
	return chipinfo_get_ctxt()->raw_package_type;
}

chipinfo_result_t chipinfo_get_num_cpu_cores(uint32_t cpu_cluster,
							uint32_t *cores)
{
	if (cores == NULL) {
		return CHIPINFO_ERROR_INVALID_PARAMETER;
	}

	if (!chipinfo_get_ctxt()->init_complete) {
		return CHIPINFO_ERROR_NOT_INITIALIZED;
	}

	if (cpu_cluster >= chipinfo_get_ctxt()->num_clusters) {
		return CHIPINFO_ERROR_OUT_OF_RANGE_PARAMETER;
	}

	*cores = chipinfo_get_ctxt()->num_cores[cpu_cluster];

	return CHIPINFO_SUCCESS;
}
