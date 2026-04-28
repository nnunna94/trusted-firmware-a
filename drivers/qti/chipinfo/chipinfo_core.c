/*
 * Copyright (c) 2026, Qualcomm Technologies, Inc. and/or its subsidiaries.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "chipinfo.h"
#include "chipinfo_image.h"
#include "chipinfo_local.h"
#include "platforminfo_defs.h"

static chipinfo_ctxt_t chipinfo_ctxt;

chipinfo_ctxt_t *chipinfo_get_ctxt(void)
{
	return &chipinfo_ctxt;
}

chipinfo_result_t chipinfo_init(void)
{
	uint32_t *cpu_clusters, *disabled_parts, *cpu_cores_array;
	uint32_t cluster_array_size;
	platforminfo_smem_t *smem;

	if (chipinfo_ctxt.init_complete) {
		return CHIPINFO_SUCCESS;
	}

	smem = chipinfo_get_soc_info();
	if (smem == NULL) {
		/* This will only happen in pre-sil on a standalone build.
		 * Return SUCCESS using the UNKNOWN values for each field,
		 * and let the information be set manually. However, mark the driver as
		 * initialized, to record that this has happened.
		 */
		chipinfo_strlcpy(chipinfo_ctxt.chip_id_string, "UNKNOWN",
					CHIPINFO_MAX_ID_LENGTH);
		chipinfo_ctxt.init_complete = true;
		return CHIPINFO_SUCCESS;
	}

	chipinfo_ctxt.raw_part_num = smem->raw_chip_id;
	chipinfo_ctxt.chipinfo_id = (chipinfo_id_t)smem->chip_id;
	chipinfo_ctxt.rev_number = smem->raw_chip_version;
	chipinfo_ctxt.version = smem->chip_version;
	chipinfo_ctxt.family_id = (chipinfo_family_t)smem->chip_family;
	chipinfo_ctxt.family_device_id = smem->raw_device_number;
	chipinfo_ctxt.raw_family_id = smem->raw_device_family;
	chipinfo_ctxt.foundry_id = (chipinfo_foundry_id_t)smem->foundry_id;
	chipinfo_ctxt.serial_num = smem->chip_serial;
	chipinfo_ctxt.qfprom_chip_id = smem->qfprom_chip_id;
	chipinfo_ctxt.modem_supported = smem->modem_supported;
	chipinfo_strlcpy(chipinfo_ctxt.chip_id_string, smem->chip_id_str,
			CHIPINFO_MAX_ID_LENGTH);

	if (smem->format >= PLATFORMINFO_VERSION(0, 14)) {
		chipinfo_ctxt.num_clusters = smem->num_clusters;

		if (chipinfo_ctxt.num_clusters > 0) {
			if (chipinfo_ctxt.num_clusters > CHIPINFO_MAX_CPU_CLUSTERS) {
				chipinfo_ctxt.num_clusters = CHIPINFO_MAX_CPU_CLUSTERS;
			}

			cpu_clusters = (uint32_t *)((uintptr_t)smem +
							smem->cluster_array_offset);
			cluster_array_size =
				chipinfo_ctxt.num_clusters * sizeof(uint32_t);

			chipinfo_memcpy(chipinfo_ctxt.cpu_clusters,
					 sizeof(chipinfo_ctxt.cpu_clusters),
					 cpu_clusters,
					 cluster_array_size);
		}

		disabled_parts =
			(uint32_t *)((uintptr_t)smem +
					smem->disabled_features_array_offset);
		chipinfo_memcpy(chipinfo_ctxt.disabled_features,
				 CHIPINFO_NUM_PARTS * sizeof(uint32_t),
				 disabled_parts,
				 smem->num_parts * sizeof(uint32_t));
	}

	if (smem->format >= PLATFORMINFO_VERSION(0, 16)) {
		chipinfo_ctxt.feature_code = smem->feature_code;
		chipinfo_ctxt.p_code = smem->p_code;
	}

	if (smem->format >= PLATFORMINFO_VERSION(0, 19)) {
		chipinfo_ctxt.num_functional_clusters =
			smem->num_functional_clusters;
		chipinfo_ctxt.boot_cluster = smem->boot_cluster;
		chipinfo_ctxt.boot_core = smem->boot_core;
	}

	if (smem->format >= PLATFORMINFO_VERSION(0, 20)) {
		chipinfo_ctxt.raw_package_type = smem->raw_package_type;
	}

	if (smem->format >= PLATFORMINFO_VERSION(0, 21)) {
		disabled_parts =
			(uint32_t *)((uintptr_t)smem +
					smem->partial_features_array_offset);
		chipinfo_memcpy(chipinfo_ctxt.partial_features,
				 CHIPINFO_NUM_PARTS * sizeof(uint32_t),
				 disabled_parts,
				 smem->num_parts * sizeof(uint32_t));
	} else {
		chipinfo_memcpy(chipinfo_ctxt.partial_features,
				 CHIPINFO_NUM_PARTS * sizeof(uint32_t),
				 chipinfo_ctxt.disabled_features,
				 CHIPINFO_NUM_PARTS * sizeof(uint32_t));
	}

	if (smem->format >= PLATFORMINFO_VERSION(0, 22)) {
		cpu_cores_array = (uint32_t *)((uintptr_t)smem +
						smem->cpu_cores_array_offset);
		/* cpu_cluster_cores_array contains no.of cores per cluster for all clusters */
		cluster_array_size =
			chipinfo_ctxt.num_clusters * sizeof(uint32_t);

		chipinfo_memcpy(chipinfo_ctxt.num_cores,
					sizeof(chipinfo_ctxt.num_cores),
					cpu_cores_array,
					cluster_array_size);
	}

	chipinfo_ctxt.init_complete = true;
	return CHIPINFO_SUCCESS;
}
