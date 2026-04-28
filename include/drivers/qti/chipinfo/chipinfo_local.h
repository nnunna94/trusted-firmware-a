/*
 * Copyright (c) 2026, Qualcomm Technologies, Inc. and/or its subsidiaries.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef CHIPINFO_LOCAL_H
#define CHIPINFO_LOCAL_H

#include <stdbool.h>
#include <stdint.h>

#include "chipinfo.h"
#include "chipinfo_defs.h"

/*
 * chipinfo_t
 *
 * Container for all chip info
 */
typedef struct {
	bool init_complete; /* Driver has been initialized */
	uint32_t raw_part_num; /* JTAG ID/Device Number */
	chipinfo_id_t chipinfo_id; /* Sequential ID, unique across all chips */
	chipinfo_version_t version; /* Major and Minor versions */
	uint32_t rev_number; /* HW Revision number. e.g. 0 = v1.0, 1 = v1.1, 2 = v2.0 */
	chipinfo_family_t
		family_id; /* Internal ChipInfo family (not architectural family) */
	uint32_t family_device_id; /* Device ID within this family, unique within the family */
	uint32_t raw_family_id; /* Architectural family number */
	chipinfo_foundry_id_t
		foundry_id; /* The foundry in which the chip was manufactured */
	chipinfo_serial_num_t
		serial_num; /* Unique (within chip family) serial number */
	chipinfo_qfprom_chip_id_t
		qfprom_chip_id; /* Chip ID read from QFPROM fuses */
	chipinfo_modem_t
		modem_supported; /* Modem hardware is/isn't supported on this platform */
	char chip_id_string
		[CHIPINFO_MAX_ID_LENGTH]; /* The chip's name as a string */
	uint32_t num_clusters;
	uint32_t cpu_clusters[CHIPINFO_MAX_CPU_CLUSTERS]; /* Static array for CPU cluster bitmasks */
	uint32_t disabled_features
		[CHIPINFO_NUM_PARTS]; /* 1 or 0 for each part, marking parts as fully
							  * disabled or partially/fully enabled */
	chipinfo_feature_code_t
		feature_code; /* Feature Code enum for this device */
	chipinfo_p_code_t p_code; /* PCode enum for this device */
	uint32_t num_functional_clusters; /* Number of clusters with at least 1 functional core */
	uint32_t boot_cluster; /* zero-indexed boot cluster */
	uint32_t boot_core; /* zero-indexed boot core */
	uint32_t raw_package_type; /* Raw package type */
	uint32_t partial_features
		[CHIPINFO_NUM_PARTS]; /* Disabled features fuse value for each part */
	uint32_t num_cores[CHIPINFO_MAX_CPU_CLUSTERS]; /* Static array for number of cores per cluster */
} chipinfo_ctxt_t;

chipinfo_ctxt_t *chipinfo_get_ctxt(void);

#endif /* CHIPINFO_LOCAL_H */
