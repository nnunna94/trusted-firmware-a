/*
 * Copyright (c) 2026, Qualcomm Technologies, Inc. and/or its subsidiaries.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef CHIPINFO_H
#define CHIPINFO_H

#include <stdint.h>

#include "chipinfo_defs.h"

/*
 * Returns the version of the chip as a chipinfo_version_t
 */
chipinfo_version_t chipinfo_get_chip_version(void);

/*
 * Returns the version of the chip as read from HW.
 * This "raw" version is a strictly increasing integer counter,
 * incremented for each new HW version. e.g.:
 * v1.0 = 0
 * v1.1 = 1
 * v1.2 = 2
 * v2.0 = 3
 * ...
 */
uint32_t chipinfo_get_raw_chip_version(void);

/*
 * Returns the chip ID as a chipinfo_id_t
 */
chipinfo_id_t chipinfo_get_chip_id(void);

/*
 * Returns the chip ID as read from HW (i.e. the JTAG ID)
 */
uint32_t chipinfo_get_raw_chip_id(void);

/*
 * Returns the name of the chip (e.g. MSM8998, MDM9665)
 */
chipinfo_result_t chipinfo_get_chip_id_string(char *sz_chip_id_str,
						uint32_t max_length);

/*
 * Returns the chip's family as a chipinfo_family_t. This has no connection
 * to the architectural family - it's used to group all revisions of a specific chip
 */
chipinfo_family_t chipinfo_get_chip_family(void);

/*
 * Returns the marketing name of the processor. This is typically the same
 * as the ChipIdString.
 */
chipinfo_result_t chipinfo_get_marketing_name_string(char *marketing_name_str,
						        uint32_t max_len);

/*
 * Returns whether or not the chip supports a modem.
 */
chipinfo_modem_t chipinfo_get_modem_support(void);

/*
 * Returns the chip's serial number
 */
chipinfo_serial_num_t chipinfo_get_serial_number(void);

/*
 * Returns the foundry ID for the chip as a ChipInfoFoundryIdType
 */
chipinfo_foundry_id_t chipinfo_get_foundry_id(void);

/*
 * Returns the chip's architecture family number as read from HW
 */
uint32_t chipinfo_get_raw_device_family(void);

/*
 * Returns the chip's device ID within its architectural family, as read from HW
 */
uint32_t chipinfo_get_raw_device_number(void);

/*
 * Returns the chip's QFPROM chip ID, which can be used along with the
 * serial number to uniquely identify the chip
 */
chipinfo_qfprom_chip_id_t chipinfo_get_qfprom_chip_id(void);

/*
 * Retrieve a mask of CPUs in the current cluster which are marked as Disabled in the PTE fuses.
 * If the cluster has fewer than 32 CPUs, the additional CPUs are marked as Enabled.
 */
chipinfo_result_t chipinfo_get_disabled_cpus(uint32_t cpu_cluster,
					       uint32_t *mask);

/*
 * Get SKU and Product Code information for the current device.
 *
 * This information can be used to make functional decisions, e.g.,
 * selecting different DDR voltage plans.
 *
 * If the SKU_ID and/or P_CODE fuses are not blown on this device, the
 * value(s) returned by this function will be the corresponding
 * CHIPINFO_*_UNKNOWN value.
 */
chipinfo_result_t chipinfo_get_sku(chipinfo_sku_t *info);

/*
 * Get the number of functional clusters.
 *
 * This is the total number of functional clusters based on partial
 * binning and SKUing. A cluster is deemed functional if it has at least
 * one functional core. There will always be at least 1 functional
 * cluster: the one that's running this code.
 */
chipinfo_result_t chipinfo_get_num_functional_clusters(uint32_t *num_clusters);

/*
 * Get the boot cluster and core.
 *
 * This function returns an error type instead of returning the queried values directly
 * because there's no sentinel value it can use: while the other APIs can use a return
 * value of 0 to mean "UNKNOWN", 0 is a valid core/cluster index.
 */
chipinfo_result_t chipinfo_get_boot_cluster_and_core(uint32_t *cluster,
						       uint32_t *core);

/*
 * Initialize the ChipInfo driver
 */
chipinfo_result_t chipinfo_init(void);

/*
 * Gets the HW Part(IP) level  Specific details from the fuse
 * such as which all features are enabled/disabled in a HW Part.
 */
chipinfo_result_t chipinfo_get_disabled_features(chipinfo_part_t part,
						   uint32_t idx,
						   uint32_t *mask);

/*
 * Check if the specified Part is enabled or Disabled
 */
chipinfo_result_t chipinfo_is_part_disabled(chipinfo_part_t part,
					      uint32_t idx, bool *mask);

/*
 * Return the raw package type.
 */
uint32_t chipinfo_get_raw_package_type(void);

/*
 * Provides number of cores for requested cluster
 */
chipinfo_result_t chipinfo_get_num_cpu_cores(uint32_t cpu_cluster,
				uint32_t *cores);

#endif /* CHIPINFO_H */
