/*
 * linux/arch/arm/mach-omap2/cpuidle44xx.c
 *
 * OMAP4 CPU IDLE Routines
 *
 * Copyright (C) 2009 Texas Instruments, Inc.
 * Rajendra Nayak <rnayak@ti.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/sched.h>
#include <linux/cpuidle.h>

#include <plat/powerdomain.h>
#include <plat/clockdomain.h>

#ifdef CONFIG_CPU_IDLE

#define OMAP4_MAX_STATES 1
#define OMAP4_STATE_C1 0 /* C1 - MPU WFI + Core active */

#define wfi() \
	{	\
		__asm__ __volatile__ ("wfi");	\
	}

#define wfe() \
	{	\
		__asm__ __volatile__ ("wfe");	\
	}

struct omap4_processor_cx {
	u8 valid;
	u8 type;
	u32 sleep_latency;
	u32 wakeup_latency;
	u32 mpu_state;
	u32 core_state;
	u32 threshold;
	u32 flags;
};

struct omap4_processor_cx omap4_power_states[OMAP4_MAX_STATES];
struct omap4_processor_cx current_cx_state;

static int omap4_idle_bm_check(void)
{
	return 0;
}

/**
 * omap4_enter_idle - Programs OMAP4 to enter the specified state
 * @dev: cpuidle device
 * @state: The target state to be programmed
 *
 * Called from the CPUidle framework to program the device to the
 * specified target state selected by the governor.
 */
static int omap4_enter_idle(struct cpuidle_device *dev,
			struct cpuidle_state *state)
{
	wfi();
	local_irq_enable();
	return 0;
}

/**
 * omap4_enter_idle_bm - Checks for any bus activity
 * @dev: cpuidle device
 * @state: The target state to be programmed
 *
 * Used for C states with CPUIDLE_FLAG_CHECK_BM flag set. This
 * function checks for any pending activity and then programs the
 * device to the specified or a safer state.
 */
static int omap4_enter_idle_bm(struct cpuidle_device *dev,
			       struct cpuidle_state *state)
{
	struct cpuidle_state *new_state = state;

	if ((state->flags & CPUIDLE_FLAG_CHECK_BM) && omap4_idle_bm_check()) {
		BUG_ON(!dev->safe_state);
		new_state = dev->safe_state;
	}

	dev->last_state = new_state;
	return omap4_enter_idle(dev, new_state);
}

DEFINE_PER_CPU(struct cpuidle_device, omap4_idle_dev);

/* omap4_init_power_states - Initialises the OMAP4 specific C states.
 *
 * Below is the desciption of each C state.
 * 	C1 . MPU WFI + Core active
 */
void omap_init_power_states(void)
{
	/* C1 . MPU WFI + Core active */
	omap4_power_states[OMAP4_STATE_C1].valid = 1;
	omap4_power_states[OMAP4_STATE_C1].type = OMAP4_STATE_C1;
	omap4_power_states[OMAP4_STATE_C1].sleep_latency = 2;
	omap4_power_states[OMAP4_STATE_C1].wakeup_latency = 2;
	omap4_power_states[OMAP4_STATE_C1].threshold = 5;
	omap4_power_states[OMAP4_STATE_C1].mpu_state = PWRDM_POWER_ON;
	omap4_power_states[OMAP4_STATE_C1].core_state = PWRDM_POWER_ON;
	omap4_power_states[OMAP4_STATE_C1].flags = CPUIDLE_FLAG_TIME_VALID;

}

struct cpuidle_driver omap4_idle_driver = {
	.name = 	"omap4_idle",
	.owner = 	THIS_MODULE,
};

/**
 * omap4_idle_init - Init routine for OMAP4 idle
 *
 * Registers the OMAP4 specific cpuidle driver with the cpuidle
 * framework with the valid set of states.
 */
int __init omap4_idle_init(void)
{
	int cpu_id, i, count = 0;
	struct omap4_processor_cx *cx;
	struct cpuidle_state *state;
	struct cpuidle_device *dev;

	omap_init_power_states();
	cpuidle_register_driver(&omap4_idle_driver);

	for_each_cpu(cpu_id, cpu_online_mask) {
		pr_err("CPUidle for CPU%d registered\n", cpu_id);
		dev = &per_cpu(omap4_idle_dev, cpu_id);
		dev->cpu = cpu_id;
		count = 0;
		for (i = OMAP4_STATE_C1; i < OMAP4_MAX_STATES; i++) {
			cx = &omap4_power_states[i];
			state = &dev->states[count];

			if (!cx->valid)
				continue;
			cpuidle_set_statedata(state, cx);
			state->exit_latency = cx->sleep_latency +
							cx->wakeup_latency;
			state->target_residency = cx->threshold;
			state->flags = cx->flags;
			state->enter = (state->flags & CPUIDLE_FLAG_CHECK_BM) ?
				omap4_enter_idle_bm : omap4_enter_idle;
			if (cx->type == OMAP4_STATE_C1)
				dev->safe_state = state;
			sprintf(state->name, "C%d", count+1);
			count++;
		}

		if (!count)
			return -EINVAL;
		dev->state_count = count;

		if (cpuidle_register_device(dev)) {
			printk(KERN_ERR "%s: CPUidle register device failed\n",
				__func__);
			return -EIO;
		}
	}

	return 0;
}
#else
int __init omap4_idle_init(void)
{
	return 0;
}
#endif /* CONFIG_CPU_IDLE */
late_initcall(omap4_idle_init);


