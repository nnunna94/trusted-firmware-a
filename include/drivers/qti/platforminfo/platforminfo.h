/*
 * Copyright (c) 2026, Qualcomm Technologies, Inc. and/or its subsidiaries.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef PLATFORMINFO_H
#define PLATFORMINFO_H

#include <stdint.h>

#include "platforminfo_defs.h"

/**
 * Initializes the PlatformInfo driver.
 *
 * Reads platform information from SMEM and caches it in the driver
 * context. Subsequent calls are no-ops if initialization has already
 * succeeded.
 *
 * @return
 *   PLATFORMINFO_SUCCESS        -- Initialization succeeded.
 *   PLATFORMINFO_ERROR_NOT_FOUND -- SMEM item was not found or is empty.
 *   PLATFORMINFO_ERROR_INVALID_DATA -- KVP offset in SMEM is out of bounds.
 */
platforminfo_result_t platforminfo_init(void);

/**
 * Returns the target platform type.
 *
 * Identifies the hardware platform on which the software is running
 * (e.g., MTP, QRD, CDP).
 *
 * @return
 *   platforminfo_platform_type_t value for the current target.
 *   Returns PLATFORMINFO_TYPE_UNKNOWN if the driver has not been
 *   initialized.
 *
 * @dependencies
 *   platforminfo_init() must have been called prior to this function.
 */
platforminfo_platform_type_t platforminfo_get_platform(void);

/**
 * Returns the hardware revision of the platform board.
 *
 * Within a given platform type (e.g., MTP, QRD), multiple board
 * revisions may exist to reflect minor hardware reworks or component
 * changes. The version is encoded as PLATFORMINFO_VERSION(major, minor)
 * where major and minor track successive hardware deltas on the same
 * board design.
 *
 * @return
 *   Hardware revision of the current platform board.
 *
 * @dependencies
 *   platforminfo_init() must have been called prior to this function.
 */
uint32_t platforminfo_get_version(void);

/**
 * Returns the platform subtype.
 *
 * The subtype further qualifies the platform type to distinguish
 * between hardware variants within the same platform family.
 *
 * @return
 *   Platform subtype of the current target.
 *
 * @dependencies
 *   platforminfo_init() must have been called prior to this function.
 */
uint32_t platforminfo_get_subtype(void);

/**
 * Returns the value associated with a given platform key.
 *
 * Key-value pairs (KVPs) are stored in SMEM alongside the platform
 * information and encode optional per-platform configuration data
 * (e.g., DDR frequency, GFX frequency).
 *
 * @param[in]  key    Key to look up; must be a valid platforminfo_key_type_t.
 * @param[out] value  Pointer to a uint32_t that receives the value on success.
 *                    Must not be NULL.
 *
 * @return
 *   PLATFORMINFO_SUCCESS                  -- Key was found; *value is valid.
 *   PLATFORMINFO_ERROR_INVALID_PARAMETER  -- value pointer is NULL.
 *   PLATFORMINFO_ERROR                    -- Key was not found in the KVP table.
 *
 * @dependencies
 *   platforminfo_init() must have been called prior to this function.
 */
platforminfo_result_t
platforminfo_get_key_value(platforminfo_key_type_t key, uint32_t *value);

/**
 * Returns the OEM Variant ID of the platform.
 *
 * The OEM Variant ID is used to distinguish between different OEM
 * customizations of the same hardware platform. Available from
 * SMEM format version PLATFORMINFO_SMEM_VERSION_OEM_VARIANT onwards;
 * returns 0 for older SMEM formats.
 *
 * @return
 *   OEM Variant ID of the current target, or 0 if not available.
 *
 * @dependencies
 *   platforminfo_init() must have been called prior to this function.
 */
uint32_t platforminfo_get_oem_variant_id(void);

#endif