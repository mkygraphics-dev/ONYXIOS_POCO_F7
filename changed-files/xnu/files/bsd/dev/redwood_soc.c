/*
 * The SoC: which phone this kernel is talking to, and where its parts are.
 *
 * The addresses are the phone's own, read out of its device tree. The loader
 * brings the phone up and knows which one it is: it says so on the kernel's
 * command line ("soc=onyx"), and this is where that is read.
 */

#include <sys/redwood_soc.h>
#include <pexpert/pexpert.h>
#include <string.h>
#include <sys/systm.h>

/*
 * The POCO X5 Pro 5G (redwood): the Snapdragon 778G, SM7325.
 * Every address here is the one the drivers used before there was more than
 * one phone; they are kept as they were, so that nothing changes for redwood.
 */
static const struct redwood_soc redwood_profile = {
	.name = "redwood",
	.soc_base = 0x00400000UL,
	.gic_dist = 0x17A00000UL,
	.gic_redist = 0x17A60000UL,
	.gic_its = 0,
	.cores = 8,
	.core_shift = 8,
	.small_cores = 0x0f,
	.smem_phys = 0x80900000UL,
	.smem_size = 0x200000UL,
	.cmd_db_phys = 0x80860000UL,
	.cmd_db_size = 0x20000UL,
	.rsc_phys = 0x18220000UL,
	.rsc_size = 0x10000UL,
	.smem_lock_phys = 0x01F43000UL,
	.epss_phys = 0x18591000UL,
	.epss_domain_size = 0x1000UL,
	.epss_domains = 3,
	.epss_domain_of = { 0, 0, 0, 0, 1, 1, 1, 2 },
	.gcc_phys = 0x00100000UL,
	.gcc_size = 0x80000UL,
	.tlmm_phys = 0x0F100000UL,
	.tlmm_size = 0x100000UL,
	.pdc_phys = 0x0B290000UL,
	.qfprom_phys = 0x00780000UL,
	.gpu_phys = 0x03D00000UL,
	.gpu_size = 0x40000UL,
	.gpucc_phys = 0x03D90000UL,
	.gpucc_size = 0x9000UL,
	.gpu_smmu_phys = 0x03DA0000UL,
	.gpu_chip_id = 0x06030500UL,
	.gpu_gmu_phys = 0x03D6A000UL,
	.gpu_gmu_size = 0x34000UL,
	.gpu_gbif_phys = 0x03D0F000UL,
	.gpu_gbif_size = 0x1000UL,
	.gpu_cx_misc_phys = 0x03D9E000UL,
	.gpu_cx_misc_size = 0x1000UL,
	.gpu_rscc_phys = 0x03DE0000UL,
	.gpu_rscc_size = 0x2000UL,
	.gpu_tsens_phys = 0x0C265000UL,
	.gpu_tsens_size = 0x200UL,
	.dispcc_phys = 0x0AF00000UL,
	.dispcc_size = 0x2000UL,
	.disp_intf_phys = 0x0AE36000UL,
	.camcc_phys = 0x0AD00000UL,
	.camcc_size = 0x20000UL,
	.arb_core_phys = 0x0C440000UL,
	.arb_core_size = 0x1100UL,
	.arb_cnfg_phys = 0x0C40A000UL,
	.arb_cnfg_size = 0x1000UL,
	.arb_observer_phys = 0x0E600000UL,
	.arb_observer_size = 0x10000UL,
	.arb_chnls_phys = 0x0C600000UL,
	.ufshc_phys = 0x1D84000UL,
	.ufshc_size = 0x3000UL,
	.ipcc_phys = 0x00408000UL,
	.ipcc_size = 0x1000UL,
	.aoss_phys = 0x0C300000UL,
	.aoss_size = 0x400UL,
	.apps_smmu_phys = 0x15000000UL,
	.wlan_mem_phys = 0x9AE00000UL,
	.wlan_mem_size = 0x01900000UL,
	.adsp_mem_phys = 0x86700000UL,
	.adsp_mem_size = 0x04000000UL,
	.lpass_va_phys = 0x03370000UL,
	.lpass_tx_phys = 0x03220000UL,
	.lpass_rx_phys = 0,
	.lpass_wsa_phys = 0,
	.lpass_tx_swr_phys = 0x03230000UL,
	.lpass_tx_hctl_phys = 0x032A9000UL,
	.lpass_lpi_phys = 0x033C0000UL,
	.lpass_lpi_high_phys = 0x033CE000UL,
	.lpass_lpi_slew_phys = 0x0355A000UL,
	.se_phys = 0x00A94000UL,
	.se_size = 0x4000UL,
};

/*
 * The POCO F7 (onyx): the Snapdragon 8 Elite, SM8735 ("tuna").
 *
 * The addresses are the phone's own, from the device tree of the phone's
 * Android build (crdroid's "android_kernel_xiaomi_sm8735-devicetrees"):
 *
 *   gic_dist, gic_redist, gic_its   intc@17100000: GICD 0x17100000 (0x10000),
 *                                   GICR 0x17180000 (0x200000, stride 0x40000),
 *                                   ITS 0x17140000
 *   rsc_phys                        apps_rsc rsc@17a00000: DRV 2 is at 0x17a20000
 *   epss_phys, epss_domains         qcom,cpufreq-epss: 0x17d91000, four domains
 *                                   of 0x1000 (freq-domain0 ... freq-domain3)
 *   smem_phys                       smem_region@81d00000, 0x200000
 *   cmd_db_phys                     aop_cmd_db_region@81c60000, 0x20000
 *   tlmm_phys                       pinctrl@f000000
 *   gcc_phys                        clock-controller@100000
 *   qfprom_phys                     qfprom@221c8000
 *   pdc_phys                        interrupt-controller@b220000
 *   gpu_phys                        qcom,kgsl-3d0@3d00000
 *   gpucc_phys                      clock-controller@3d90000 (0x9800)
 *   gpu_smmu_phys                   kgsl-smmu@3da0000
 *   dispcc_phys                     clock-controller@af00000
 *   disp_intf_phys                  mdp_intf_base of the first DSI: 0x0ae36000
 *   camcc_phys                      clock-controller@ade0000 (0x20000)
 *   ufshc_phys                      ufshc@1d84000
 *   ipcc_phys                       qcom,ipcc@406000
 *   aoss_phys                       power-controller@c300000
 *   wlan_mem_phys                   wlan_msa_region@a6400000 (0xc00000)
 *   adsp_mem_phys                   adspslpi_region@99e00000 (0x4080000)
 *   lpass_*                         lpass-cdc of the SoC's audio: va-macro@7660000,
 *                                   tx-macro@6AE0000, rx-macro@6AC0000,
 *                                   wsa-macro@6B00000, lpi_pinctrl@07760000
 *
 * The arbiter (the cameras and the torch) is not in the phone's public device
 * tree: those addresses are in the vendor's own tree, and have to be read off
 * the phone (see docs/PORTING-onyx.md). They are zero until then.
 */
static const struct redwood_soc onyx_profile = {
	.name = "onyx",
	.soc_base = 0x00400000UL,
	.gic_dist = 0x17100000UL,
	.gic_redist = 0x17180000UL,
	.gic_its = 0x17140000UL,
	.cores = 8,
	.core_shift = 8,
	/*
	 * Cores 0 to 6 are alike (capacity-dmips-mhz 1024) and core 7 is the
	 * prime one (1157), so the seven others are the "slow" ones.
	 */
	.small_cores = 0x7f,
	.smem_phys = 0x81D00000UL,
	.smem_size = 0x200000UL,
	.cmd_db_phys = 0x81C60000UL,
	.cmd_db_size = 0x20000UL,
	.rsc_phys = 0x17A20000UL,
	.rsc_size = 0x10000UL,
	.smem_lock_phys = 0,
	.epss_phys = 0x17D91000UL,
	.epss_domain_size = 0x1000UL,
	.epss_domains = 4,
	/*
	 * dt: "qcom,freq-domain" of each core: cores 0 and 1 are in domain 0,
	 * cores 5 and 6 in domain 1, the prime core 7 in domain 2, and cores 2,
	 * 3 and 4 in domain 3.
	 */
	.epss_domain_of = { 0, 0, 3, 3, 3, 1, 1, 2 },
	.gcc_phys = 0x00100000UL,
	.gcc_size = 0x80000UL,
	.tlmm_phys = 0x0F100000UL,
	.tlmm_size = 0x100000UL,
	.pdc_phys = 0x0B220000UL,
	.qfprom_phys = 0x221C8000UL,
	.gpu_phys = 0x03D00000UL,
	.gpu_size = 0x40000UL,
	.gpucc_phys = 0x03D90000UL,
	.gpucc_size = 0x9800UL,
	.gpu_smmu_phys = 0x03DA0000UL,
	/*
	 * The Snapdragon 8 Elite's graphics is an a7xx; this driver knows only the
	 * a6xx of the Snapdragon 778G, and says so rather than write to registers
	 * that mean something else.
	 */
	.gpu_chip_id = 0,
	/*
	 * The a7xx has its own registers, in its own places: they are not in the
	 * phone's public device tree, and the driver that knows them (an a6xx
	 * driver) refuses to run on this chip anyway. They are zero until a driver
	 * that knows this chip writes them down.
	 */
	.gpu_gmu_phys = 0,
	.gpu_gmu_size = 0,
	.gpu_gbif_phys = 0,
	.gpu_gbif_size = 0,
	.gpu_cx_misc_phys = 0,
	.gpu_cx_misc_size = 0,
	.gpu_rscc_phys = 0,
	.gpu_rscc_size = 0,
	.gpu_tsens_phys = 0,
	.gpu_tsens_size = 0,
	.dispcc_phys = 0x0AF00000UL,
	.dispcc_size = 0x2000UL,
	.disp_intf_phys = 0x0AE36000UL,
	.camcc_phys = 0x0ADE0000UL,
	.camcc_size = 0x20000UL,
	.arb_core_phys = 0,
	.arb_core_size = 0,
	.arb_cnfg_phys = 0,
	.arb_cnfg_size = 0,
	.arb_observer_phys = 0,
	.arb_observer_size = 0,
	.arb_chnls_phys = 0,
	.ufshc_phys = 0x1D84000UL,
	.ufshc_size = 0x3000UL,
	.ipcc_phys = 0x00406000UL,
	.ipcc_size = 0x1000UL,
	.aoss_phys = 0x0C300000UL,
	.aoss_size = 0x400UL,
	.apps_smmu_phys = 0x15000000UL,
	.wlan_mem_phys = 0xA6400000UL,
	.wlan_mem_size = 0xC00000UL,
	.adsp_mem_phys = 0x99E00000UL,
	.adsp_mem_size = 0x4080000UL,
	.lpass_va_phys = 0x07660000UL,
	.lpass_tx_phys = 0x06AE0000UL,
	.lpass_rx_phys = 0x06AC0000UL,
	.lpass_wsa_phys = 0x06B00000UL,
	/*
	 * The SoundWire master of the TX macro, its HCTL and the higher pins of
	 * the LPASS are not in the phone's public device tree: they are to be read
	 * off the phone (see docs/PORTING-onyx.md). The pins and their slew rates
	 * are one block here (lpi_pinctrl@7760000).
	 */
	.lpass_tx_swr_phys = 0,
	.lpass_tx_hctl_phys = 0,
	.lpass_lpi_phys = 0x07760000UL,
	.lpass_lpi_high_phys = 0,
	.lpass_lpi_slew_phys = 0x07760000UL,
	.se_phys = 0,
	.se_size = 0,
};

const struct redwood_soc *
redwood_soc(void)
{
	static const struct redwood_soc *soc;
	char name[32];

	/*
	 * Two callers at once would both find the same phone: the answer does
	 * not change, so there is nothing to lock.
	 */
	if (soc != NULL) {
		return soc;
	}

	soc = &redwood_profile;

	if (PE_parse_boot_argn("soc", name, sizeof(name)) && name[0] != '\0') {
		if (strcmp(name, "onyx") == 0) {
			soc = &onyx_profile;
		} else {
			printf("redwood_soc: no phone called \"%s\": this is redwood\n", name);
		}
	}

	return soc;
}
