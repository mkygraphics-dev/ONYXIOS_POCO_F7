/*
 * onyx: paste into xnuloader/loader.c next to platform_redwood and add &platform_onyx to platforms[].
 * Every value is explained in docs/PORTING.md ("The loader").
 *
 * The values are the POCO F7's own: they come from the device tree of its
 * Android build (crdroid's "android_kernel_xiaomi_sm8735-devicetrees", branch
 * 17.0), the same tree the kernel's SoC profile is written from.
 */
static const struct platform platform_onyx = {
	.name = "onyx",
	/*
	 * One window over the SoC's peripherals. Everything the kernel touches
	 * is below 4 GB (the highest is the UFS host controller at 0x1d84000),
	 * so the same window as redwood's does.
	 */
	.io_base = 0x00400000, .io_size = 0x3FC00000,
	/*
	 * dt: "intc@17100000", compatible "arm,gic-v3":
	 *   reg = <0x17100000 0x10000>              the distributor
	 *   reg = <0x17180000 0x200000>             the redistributors, 0x40000 apart
	 * There are eight of them, one for each core, hence 0x200000 and not the
	 * 0x100000 redwood needs.
	 */
	.gic_dist = 0x17100000, .gic_redist = 0x17180000,
	.gic_dist_size = 0x10000, .gic_redist_size = 0x200000,
	.uart = 0,                                      /* phones: no PL011 */
	.log_other_cpu = 1,
	/*
	 * dt: "cpus": eight cores, numbered in the second affinity field of MPIDR
	 * (reg = <0x0 0x100>, <0x1 0x100> ... <0x7 0x100>), so the shift is 8.
	 */
	.cores = 8, .core_shift = 8,
	/*
	 * Cores 0 to 6 are alike (capacity-dmips-mhz 1024) and core 7 is the prime
	 * one (1157), so the seven others are the "slow" ones.
	 */
	.small_cores = 0x7f,
	.fb_base = 0x0,                                 /* TODO: the framebuffer base the UEFI prints ("display ... at") */
};
