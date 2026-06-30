/*
 * Copyright (c) 2026 Qualcomm Technologies, Inc. and/or its subsidiaries.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Low-level clock register accessors for the QTI clock driver.
 *
 * These routines used to live behind a separate HAL abstraction (HALclk.c).
 * They are now folded directly into the driver and operate on the plain
 * descriptor structures declared in <drivers/qti/clock/clock_types.h>.
 */

#include <stdbool.h>
#include <stdint.h>

#include <drivers/delay_timer.h>
#include <drivers/qti/clock/clock_driver.h>
#include <drivers/qti/clock/clock_types.h>
#include <lib/mmio.h>

void clock_hal_enable_clock(struct clock_clk_desc *clock)
{
	uintptr_t addr;
	uint32_t val, mask;

	if (clock->vote_reg.addr != 0U) {
		addr = clock->vote_reg.addr;
		mask = clock->vote_reg.mask;
	} else {
		addr = clock->cbcr_addr;
		mask = HAL_CLK_BRANCH_CTRL_REG_CLK_ENABLE_FMSK;
	}

	if (addr != 0U) {
		val = mmio_read_32(addr);
		/* Note if TF-A enabled this clock. */
		clock->tfa_enabled = ((val & mask) == 0U);
		mmio_write_32(addr, val | mask);
	}
}

void clock_hal_disable_clock(struct clock_clk_desc *clock)
{
	uintptr_t addr;
	uint32_t mask;

	if (clock->vote_reg.addr != 0U) {
		addr = clock->vote_reg.addr;
		mask = clock->vote_reg.mask;
	} else {
		addr = clock->cbcr_addr;
		mask = HAL_CLK_BRANCH_CTRL_REG_CLK_ENABLE_FMSK;
	}

	if (addr != 0U) {
		mmio_write_32(addr, mmio_read_32(addr) & ~mask);
	}

	clock->tfa_enabled = false;
}

void clock_hal_enable_source(struct clock_source_desc *source)
{
	uintptr_t addr;

	/* Only votable PLLs are supported. */
	if (source->vote_reg.addr == 0U) {
		return;
	}

	mmio_setbits_32(source->vote_reg.addr, source->vote_reg.mask);

	addr = source->mode_addr;
	if (addr != 0U) {
		while ((mmio_read_32(addr) &
			HAL_CLK_PLL_MODE_PLL_LOCK_DET_BMSK) == 0U) {
		}
	}
}

int clock_hal_is_clock_on(struct clock_clk_desc *clock)
{
	if (clock->cbcr_addr == 0U) {
		return 0;
	}

	if ((mmio_read_32(clock->cbcr_addr) &
	     HAL_CLK_BRANCH_CTRL_REG_CLK_OFF_FMSK) != 0U) {
		return -1;
	}

	return 0;
}

int clock_hal_wait_for_clock_on(struct clock_clk_desc *clock)
{
	uint32_t retry = 100U;

	if (clock->cbcr_addr == 0U) {
		return -1;
	}

	/*
	 * Skip polling if HW_CTL is set, since the clock will only enable
	 * based on traffic and/or usage.
	 */
	if ((mmio_read_32(clock->cbcr_addr) &
	     HAL_CLK_BRANCH_CTRL_REG_CLK_HW_CTL_FMSK) != 0U) {
		return 0;
	}

	while ((clock_hal_is_clock_on(clock) != 0) && (--retry > 0U)) {
		udelay(1U);
	}

	if (retry == 0U) {
		return -1;
	}

	return 0;
}

void clock_hal_enable_power_domain(struct clock_power_domain_desc *power_domain)
{
	uintptr_t addr;
	uint32_t val, mask;

	if (power_domain->vote_reg.addr != 0U) {
		/* Vote-based (GDS_HW) enable — set the vote bit. */
		addr = power_domain->vote_reg.addr;
		mask = power_domain->vote_reg.mask;
		power_domain->tfa_enabled =
			((mmio_read_32(addr) & mask) == 0U);
		mmio_setbits_32(addr, mask);
	} else {
		addr = power_domain->gdscr_addr;
		mask = HAL_CLK_GDSCR_SW_COLLAPSE_FMSK;
		if (addr != 0U) {
			val = mmio_read_32(addr);
			power_domain->tfa_enabled = ((val & mask) != 0U);
			mmio_write_32(addr, val & ~mask);
		}
	}
}

void clock_hal_disable_power_domain(struct clock_power_domain_desc *power_domain)
{
	if (power_domain->vote_reg.addr != 0U) {
		/* Vote-based (GDS_HW) disable — clear the vote bit. */
		mmio_clrbits_32(power_domain->vote_reg.addr,
				power_domain->vote_reg.mask);
	} else if (power_domain->gdscr_addr != 0U) {
		mmio_setbits_32(power_domain->gdscr_addr,
				HAL_CLK_GDSCR_SW_COLLAPSE_FMSK);
	}

	power_domain->tfa_enabled = false;
}

int clock_hal_is_power_domain_on(struct clock_power_domain_desc *power_domain)
{
	if (power_domain->vote_reg.addr != 0U) {
		/*
		 * Vote-based domain: on if our vote bit is set. No separate
		 * power-up-complete status to poll.
		 */
		return ((mmio_read_32(power_domain->vote_reg.addr) &
			 power_domain->vote_reg.mask) != 0U) ? 0 : -1;
	}

	if (power_domain->gdscr_addr == 0U) {
		return -1;
	}

	/*
	 * Non-votable GDSCR: poll GDSC_POWER_UP_COMPLETE in the CFG_GDSCR
	 * (architecturally at GDSCR+4), which reflects sequencer completion
	 * rather than just the PWR_ON latch.
	 */
	return ((mmio_read_32(power_domain->gdscr_addr + 4U) &
		 HAL_CLK_CFG_GDSCR_POWER_UP_COMPLETE_FMSK) != 0U) ? 0 : -1;
}

int clock_hal_wait_for_power_domain_on(struct clock_power_domain_desc *power_domain)
{
	uint32_t retry = 500U;

	/*
	 * Wait at least 8 XO cycles before checking the status, since it is
	 * not reliable immediately after powering on the domain.
	 */
	udelay(1U);

	while ((clock_hal_is_power_domain_on(power_domain) != 0) &&
	       (--retry > 0U)) {
		udelay(1U);
	}

	if (retry == 0U) {
		return -1;
	}

	return 0;
}
