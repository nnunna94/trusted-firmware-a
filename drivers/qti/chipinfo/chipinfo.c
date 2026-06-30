/*
 * Copyright (c) 2026, Qualcomm Technologies, Inc. and/or its subsidiaries.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <drivers/qti/chipinfo/chipinfo.h>
#include <drivers/qti/platforminfo/platforminfodefs.h>
#include <drivers/qti/smem/smem.h>

#include "chipinfo_internal.h"

static struct chipinfo_ctxt chipinfo_ctxt;

uint32_t chipinfo_get_chip_version(void)
{
	if (!chipinfo_ctxt.initialized) {
		return CHIPINFO_VERSION_UNKNOWN;
	}

	return chipinfo_ctxt.version;
}

enum chipinfo_id chipinfo_get_chip_id(void)
{
	if (!chipinfo_ctxt.initialized) {
		return CHIPINFO_ID_UNKNOWN;
	}

	return chipinfo_ctxt.chipinfo_id;
}

enum chipinfo_family chipinfo_get_chip_family(void)
{
	if (!chipinfo_ctxt.initialized) {
		return CHIPINFO_FAMILY_UNKNOWN;
	}

	return chipinfo_ctxt.family_id;
}

bool chipinfo_is_part_disabled(enum chipinfo_part part, uint32_t part_idx)
{
	uint32_t i;

	if (!chipinfo_ctxt.initialized) {
		return false;
	}

	if ((part == CHIPINFO_PART_UNKNOWN) ||
	    ((uint32_t)part >= CHIPINFO_NUM_PARTS)) {
		return false;
	}

	if (part_idx == 0U) {
		/*
		 * Index 0: use the flat disabled-features array which reflects
		 * the overall part fuse state reported by XBL.
		 */
		return chipinfo_ctxt.disabled_features[part] != 0U;
	}

	/*
	 * Non-zero index: search the per-instance Qultivate table for a
	 * matching (part, instance) entry. Returns false (assume present) if
	 * the table is absent (format < 23) or no entry is found.
	 */
	for (i = 0U; i < chipinfo_ctxt.num_part_info; i++) {
		const struct platforminfo_part_info *entry =
			&chipinfo_ctxt.part_info[i];

		if (((uint32_t)entry->part == (uint32_t)part) &&
		    (entry->instance == (uint8_t)part_idx)) {
			return entry->disabled != 0U;
		}
	}

	return false;
}

enum chipinfo_result qti_chipinfo_init(void)
{
	struct platforminfo_smem *smem;
	uint32_t size;
	uint32_t fmt;
	uint32_t chip_id;
	uint32_t chip_family;
	uint32_t i;

	/* Access the socinfo SMEM region populated by the boot firmware. */
	smem = (struct platforminfo_smem *)smem_get_addr(SMEM_HW_SW_BUILD_ID,
							 &size);
	if (smem == NULL || size < sizeof(uint32_t)) {
		return CHIPINFO_ERROR_NOT_FOUND;
	}

	/*
	 * Use the SMEM format version to guard field access.
	 * Each version number corresponds to a specific set
	 * of fields that XBL populated; accessing beyond what XBL wrote would
	 * read uninitialised data. The version numbers match go/platforminfo.
	 * chip_id and chip_version are present from version 1 onwards.
	 * chip_family was added in version 12.
	 * disabled_features_array_offset was added in version 14.
	 */
	fmt = smem->format;

	if (fmt < 1U) {
		return CHIPINFO_ERROR_INVALID_DATA;
	}

	/* Bound the raw values before casting into the driver enums. */
	chip_id = smem->chip_id;
	if (chip_id >= CHIPINFO_NUM_IDS) {
		chip_id = CHIPINFO_ID_UNKNOWN;
	}
	chipinfo_ctxt.chipinfo_id = (enum chipinfo_id)chip_id;
	chipinfo_ctxt.version = smem->chip_version;

	if (fmt >= PLATFORMINFO_FORMAT_VER_12) {
		chip_family = smem->chip_family;
		if (chip_family >= CHIPINFO_NUM_FAMILIES) {
			chip_family = CHIPINFO_FAMILY_UNKNOWN;
		}
		chipinfo_ctxt.family_id = (enum chipinfo_family)chip_family;
	}

	/*
	 * Read the disabled-features array. The offset is from the start of
	 * the SMEM buffer; each entry is a uint32_t, non-zero means disabled.
	 * Entries beyond num_parts stay zero (assume present).
	 */
	if (fmt >= PLATFORMINFO_FORMAT_VER_14) {
		uint32_t offset = smem->disabled_features_array_offset;
		uint32_t num = smem->num_parts;
		const uint32_t *features;

		if (num > CHIPINFO_NUM_PARTS) {
			num = CHIPINFO_NUM_PARTS;
		}

		if ((offset != 0U) && (num != 0U) &&
		    ((offset + num * sizeof(uint32_t)) <= size)) {
			features = (const uint32_t *)
				   ((uintptr_t)smem + offset);
			for (i = 0U; i < num; i++) {
				chipinfo_ctxt.disabled_features[i] =
					features[i];
			}
		}
	}

	/*
	 * Read the per-instance Qultivate table (format >= 23). The table is
	 * an array of platforminfo_part_info entries at anPartInstancesOffset.
	 * We retain a pointer into the SMEM buffer rather than copying.
	 */
	if (fmt >= PLATFORMINFO_FORMAT_VER_23) {
		uint32_t offset = smem->anPartInstancesOffset;
		uint32_t num = smem->nNumPartInstances;

		if ((offset != 0U) && (num != 0U) &&
		    ((offset + num * sizeof(struct platforminfo_part_info))
		     <= size)) {
			chipinfo_ctxt.part_info =
				(const struct platforminfo_part_info *)
				((uintptr_t)smem + offset);
			chipinfo_ctxt.num_part_info = num;
		}
	}

	chipinfo_ctxt.initialized = true;

	return CHIPINFO_SUCCESS;
}
