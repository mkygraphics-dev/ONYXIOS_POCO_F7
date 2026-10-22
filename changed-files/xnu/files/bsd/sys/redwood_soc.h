/*
 * The SoC: which phone this kernel is talking to, and where its parts are.
 *
 * One kernel runs on every phone, so an address cannot be compiled in: the loader
 * knows which phone it is bringing up, says so on the kernel's command line
 * ("soc=onyx"), and this is where that is read.
 *
 * Everything here was taken from the phone's own device tree, not from a guess:
 * every field says where it came from. A field that could not be found is zero,
 * and the driver that wants it says so.
 */

#ifndef _REDWOOD_SOC_H
#define _REDWOOD_SOC_H

#include <stdint.h>

struct redwood_soc {
	const char *name;

	/* where the loader says the phone's peripherals begin */
	unsigned long soc_base;

	/* the interrupt controller (dt: "intc", GICv3) */
	unsigned long gic_dist;     /* the distributor's registers */
	unsigned long gic_redist;   /* the redistributors' registers */
	unsigned long gic_its;      /* the interrupt translation service */

	/* the processors (dt: "cpus") */
	uint32_t cores;             /* how many of them */
	uint32_t core_shift;        /* in which field of MPIDR they are numbered */
	uint32_t small_cores;       /* a bit for each of the slower ones */

	/* the memory the processors share (dt: "reserved-memory") */
	unsigned long smem_phys;
	unsigned long smem_size;
	unsigned long cmd_db_phys;  /* dt: "aop_cmd_db_region" */
	unsigned long cmd_db_size;
	unsigned long rsc_phys;     /* dt: "apps_rsc", DRV 2 */
	unsigned long rsc_size;
	unsigned long smem_lock_phys;

	/* the clock and the power */
	unsigned long epss_phys;     /* dt: "qcom,cpufreq-epss" */
	unsigned long epss_domain_size;
	uint32_t epss_domains;      /* how many of them */
	uint32_t epss_domain_of[8]; /* which of them each core is in (dt: "qcom,freq-domain") */
	unsigned long gcc_phys;      /* dt: "clock-controller@100000" */
	unsigned long gcc_size;
	unsigned long tlmm_phys;     /* dt: "pinctrl@f000000" */
	unsigned long tlmm_size;
	unsigned long pdc_phys;      /* dt: "interrupt-controller@b220000" */
	unsigned long qfprom_phys;   /* dt: "qfprom@221c8000" */

	/* the graphics (dt: "qcom,kgsl-3d0@3d00000", "clock-controller@3d90000") */
	unsigned long gpu_phys;
	unsigned long gpu_size;
	unsigned long gpucc_phys;
	unsigned long gpucc_size;
	unsigned long gpu_smmu_phys;
	unsigned long dispcc_phys;   /* dt: "clock-controller@af00000" */
	unsigned long dispcc_size;
	unsigned long disp_intf_phys; /* dt: "mdp_intf_base" of the first DSI */
	/*
	 * The graphics chip, as the device tree says it ("qcom,chipid"): zero when
	 * the driver that asks knows only some chips and this is not one of them.
	 */
	unsigned long gpu_chip_id;

	/*
	 * The rest of the graphics chip: the general-purpose microcode engine and
	 * the blocks around it. These are the registers of the a6xx of the
	 * Snapdragon 778G; another chip has its own elsewhere.
	 */
	unsigned long gpu_gmu_phys;
	unsigned long gpu_gmu_size;
	unsigned long gpu_gbif_phys;
	unsigned long gpu_gbif_size;
	unsigned long gpu_cx_misc_phys;
	unsigned long gpu_cx_misc_size;
	unsigned long gpu_rscc_phys;
	unsigned long gpu_rscc_size;
	unsigned long gpu_tsens_phys;
	unsigned long gpu_tsens_size;

	/* the cameras and the torch (the arbiter: not in the phone's device tree) */
	unsigned long camcc_phys;    /* dt: "clock-controller@ade00000" */
	unsigned long camcc_size;
	unsigned long arb_core_phys;
	unsigned long arb_core_size;
	unsigned long arb_cnfg_phys;
	unsigned long arb_cnfg_size;
	unsigned long arb_observer_phys;
	unsigned long arb_observer_size;
	unsigned long arb_chnls_phys;

	/* the storage (dt: "ufshc@1d84000") */
	unsigned long ufshc_phys;
	unsigned long ufshc_size;

	/* the radio */
	unsigned long ipcc_phys;     /* dt: "qcom,ipcc@406000" */
	unsigned long ipcc_size;
	unsigned long aoss_phys;     /* dt: "power-controller@c300000" */
	unsigned long aoss_size;
	unsigned long apps_smmu_phys; /* dt: "apps-smmu@15000000" */
	unsigned long wlan_mem_phys; /* dt: "wlan_msa_region" */
	unsigned long wlan_mem_size;
	unsigned long adsp_mem_phys; /* dt: "adspslpi_region" */
	unsigned long adsp_mem_size;

	/* the audio (dt: "lpass-cdc" of the SoC's audio) */
	unsigned long lpass_va_phys;
	unsigned long lpass_tx_phys;
	unsigned long lpass_rx_phys;
	unsigned long lpass_wsa_phys;
	unsigned long lpass_tx_swr_phys;   /* the TX SoundWire master */
	unsigned long lpass_tx_hctl_phys;  /* its HCTL */
	unsigned long lpass_lpi_phys;       /* the LPASS pins */
	unsigned long lpass_lpi_high_phys;  /* ... the higher ones */
	unsigned long lpass_lpi_slew_phys;  /* ... their slew rates */

	/* the secure element (dt: "qseecom") */
	unsigned long se_phys;
	unsigned long se_size;
};

/*
 * The phone this kernel runs on. The first call says which one it is, and every
 * later call says the same: the phone cannot change under a running kernel.
 */
const struct redwood_soc *redwood_soc(void);

/*
 * Whether the phone has the block at this address. An address that could not be
 * found in the phone's device tree is zero: the driver that wants such a block
 * says so and leaves it alone, rather than write to address zero.
 */
#define REDWOOD_SOC_HAS(addr) ((addr) != 0)

#endif /* _REDWOOD_SOC_H */
