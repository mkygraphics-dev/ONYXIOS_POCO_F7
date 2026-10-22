# Porting HackiOS to the POCO F7 (onyx)

The POCO F7 is a Snapdragon 8 Elite (SM8735, "tuna"): a Qualcomm phone with an unlockable bootloader. The port
follows [PORTING.md](PORTING.md), which is the general procedure; this file is what is specific to this phone and
what is still missing.

Nothing here is finished: the kernel is written for the phone's device tree, but it has never been on the phone.

## What was done

| Piece | Where | State |
|---|---|---|
| The SoC profile (the addresses) | `xnu/files/bsd/dev/redwood_soc.c`, `xnu/files/bsd/sys/redwood_soc.h` | written from the phone's device tree |
| The loader's description of the phone | `xnuloader/loader.c` (`platform_onyx`) | written, but the framebuffer base is unknown |
| The device profile (the build) | `common/devices/onyx.conf` | written, but the UEFI memory map is unknown |
| The drivers | `xnu/files/bsd/dev/redwood_*.c` | all read their addresses from the profile now |
| The UEFI port | `uefi/files/Platforms/Xiaomi/onyxPkg` | **not written** (see below) |

The kernel serves more than one phone, so an address cannot be compiled into it: the loader says which phone it is
bringing up on the kernel's command line (`soc=onyx`), and `redwood_soc()` reads that. The redwood (Snapdragon 778G)
addresses are untouched, so nothing changes for the phone that works.

## The device tree

The addresses are not guesses: they come from the device tree of the phone's own Android build:

```
git clone -b 17.0 --depth 1 https://github.com/crdroidandroid/android_kernel_xiaomi_sm8735-devicetrees
```

The files that matter are `qcom/tuna.dtsi` (the SoC), `qcom/tuna-reserved-memory.dtsi` (the memory the kernel
shares with the other processors), `qcom/msm-arm-smmu-tuna.dtsi` (the SMMUs) and `qcom/onyx-sm8735-overlay.dts`
(the phone). The phone is `sun|onyx`, model `25053PC47G`.

## The addresses

Every one of these is in the profile, with the node it came from. "0" means it could not be found and the driver
that wants it says so instead of writing to address zero.

| What | Address | From |
|---|---|---|
| GIC distributor | `0x17100000` (0x10000) | `intc@17100000` |
| GIC redistributors | `0x17180000` (0x200000, stride 0x40000: eight cores) | `intc@17100000` |
| GIC ITS | `0x17140000` | `intc@17100000` |
| CPUs | 8, MPIDR's second affinity field, shift 8 | `cpus` |
| Small cores | 0x7f (cores 0 to 6; core 7 is the prime one) | `capacity-dmips-mhz` |
| EPSS (the clock and the power) | `0x17d91000`, 4 domains of 0x1000 | `qcom,cpufreq-epss` |
| Which core is in which domain | 0, 0, 3, 3, 3, 1, 1, 2 | `qcom,freq-domain` |
| Shared memory | `0x81d00000` (0x200000) | `smem_region@81d00000` |
| The boot loader's list of supplies | `0x81c60000` (0x20000) | `aop_cmd_db_region@81c60000` |
| RPMh sequencer | `0x17a20000` (0x10000) | `apps_rsc` (DRV 2) |
| Global clock controller | `0x00100000` (0x80000) | `clock-controller@100000` |
| Pins (TLMM) | `0x0f100000` (0x100000) | `pinctrl@f000000` |
| Power controller (PDC) | `0x0b220000` | `interrupt-controller@b220000` |
| Fuses | `0x221c8000` | `qfprom@221c8000` |
| Graphics | `0x03d00000` (0x40000) | `qcom,kgsl-3d0@3d00000` |
| Graphics clock controller | `0x03d90000` (0x9800) | `clock-controller@3d90000` |
| Graphics SMMU | `0x03da0000` | `kgsl-smmu@3da0000` |
| Display clock controller | `0x0af00000` (0x2000) | `clock-controller@af00000` |
| Display interface | `0x0ae36000` | `mdp_intf_base` of the first DSI |
| Camera clock controller | `0x0ade0000` (0x20000) | `clock-controller@ade0000` |
| Storage | `0x1d84000` (0x3000) | `ufshc@1d84000` |
| The mailbox | `0x00406000` (0x1000) | `qcom,ipcc@406000` |
| The always-on processor | `0x0c300000` (0x400) | `power-controller@c300000` |
| The processors' SMMU | `0x15000000` | `apps-smmu@15000000` |
| The radio's memory | `0xa6400000` (0xc00000) | `wlan_msa_region` |
| The audio DSP's memory | `0x99e00000` (0x4080000) | `adspslpi_region` |
| Audio (VA, TX, RX, WSA macros) | `0x07660000`, `0x06ae0000`, `0x06ac0000`, `0x06b00000` | `lpass-cdc` |
| Audio pins | `0x07760000` | `lpi_pinctrl@7760000` |
| The PMIC arbiter | **0** | not in the public tree |
| The shared memory's lock | **0** | not in the public tree |
| The sound wire master and its HCTL | **0** | not in the public tree |
| The secure element | **0** | `qseecom` is a shared-dma-pool here |

## The loader

`xnuloader/loader.c` tells the phones apart by the framebuffer the UEFI reports ("display 1220 x 2712 at 0x..."), and
that is the one number the port still does not have: `platform_onyx`'s `fb_base` is 0, so the loader falls back to
`qemu-virt` and gives the kernel the wrong interrupt controller. Read the address out of the loader's log on the
phone (`common/run.sh onyx`, then `./xl log`) and put it in `common/devices/onyx.platform.c`.

Everything else the loader needs is in the device tree and already written: the window over the peripherals
(`0x00400000`, 0x3fc00000 long: the highest thing the kernel touches is the storage controller at 0x1d84000), the
GIC (the distributor at 0x17100000, the redistributors at 0x17180000 and 0x200000 long, because there are eight
cores and not four), the eight cores and the shift, and the seven small ones.

## The UEFI

This is the biggest piece left, and it is the same work as for redwood (see [PORTING.md](PORTING.md), "The UEFI"):

- `uefi/files/Platforms/Xiaomi/onyxPkg`: a copy of `redwoodPkg` with **onyx's own memory map**;
- `uefi/files/Resources/Configs/onyx.toml`: `FD_BASE`, `FD_SIZE` (where the phone's own bootloader puts the UEFI
  image) and `PAYLOAD_BASE` (free RAM for the kernel and the images);
- `uefi/files/Resources/DTBs/onyx.dtb`: the device tree, for the UEFI to boot Android;
- `uefi/files/Binaries/onyx`: the UEFI drivers of the phone's own firmware, from its `xbl.img`.

The memory map cannot be copied from redwood: the Snapdragon 8 Elite's memory is laid out differently, and a map
that is wrong puts the UEFI's page tables over memory that belongs to TrustZone. It has to come from the phone's
own `vendor_boot` image (the "memory map" its bootloader uses) and be checked against the phone's `dumpsys meminfo`
and the device tree's `reserved-memory`.

The loader's `FD_BASE`, `FD_SIZE` and `PAYLOAD_BASE` in `common/devices/onyx.conf` are redwood's for now, as a
starting point, and have to be replaced with the same numbers.

## What the kernel does and does not do

| Subsystem | On this phone |
|---|---|
| The framebuffer, the display | the addresses are in the device tree; the panel is not the phone's own (it is what the UEFI draws) |
| Touch | **not driven**: the panel is FocalTech, behind Trusted Touch (`qts,trusted-touch-io-bases = <0xa90000>`), and the driver in `redwood_goodix.c` is for the Goodix of redwood |
| The buttons | the PMIC arbiter's addresses are unknown: the driver says so and does not read the keys |
| The CPU's speed | the EPSS is in the device tree: `cpufreq=1` should work |
| The storage | the UFS controller is in the device tree: the phone's flash should work |
| The graphics | **not driven**: this is an Adreno a7xx, and the driver in `redwood_adreno.c` knows the a6xx of the Snapdragon 778G. It says so and does not write to registers that mean something else |
| The radio | the addresses are in the device tree, but the radio is a WCN786x (Wi-Fi 7) and not the WCN6750 of redwood: the firmware and the driver's own checks are to be done |
| The audio | the addresses of the macros are in the device tree, but the sound wire master and its HCTL are not: the microphone is not read. The audio DSP is a PM7550BA, not the one of redwood |
| The cameras and the torch | the PMIC arbiter's addresses are unknown: the drivers say so |

## What has to be read off the phone

| What | How |
|---|---|
| The framebuffer's base | the loader's log: `display W x H at 0x...` |
| The UEFI's memory map | the phone's `vendor_boot` image (`unpack_bootimg.py vendor_boot.img`), or the addresses the phone's own bootloader prints |
| The PMIC arbiter | the phone's own device tree (`/sys/firmware/fdt` under Android, or the vendor's kernel source) |
| The shared memory's lock | the same |
| The sound wire master and its HCTL | the same |
| The graphics chip's registers | the vendor's kernel source for SM8735 (`msm_kgsl`): the a7xx's own addresses |

## Build and run

```sh
common/build.sh onyx
common/run.sh onyx
```

`common/build.sh` reads `common/devices/onyx.conf` and needs `uefi/Mu-Silicium` (from `uefi/apply.sh`) for the
UEFI image; without it, the prebuilt redwood one is used, which is not this phone's.

The kernel's own command line is in the profile's `BOOTARGS`: `touchscreen=1 cpufreq=1 bootcpu=7 cpus=8`. `touchscreen=1`
is redwood's: there is no touch driver for this phone yet.

## The changes to the kernel

- `xnu/files/bsd/sys/redwood_soc.h`, `xnu/files/bsd/dev/redwood_soc.c`: the profile and the phone tables (new);
  `xnu/redwood.patch` registers the new file in `bsd/conf/files`.
- `xnu/files/bsd/dev/redwood_*.c`: the drivers read their addresses from the profile instead of writing them down.
  The redwood addresses are in the profile now, unchanged.
- `xnuloader/loader.c`: `platform_onyx`, the sizes of the GIC's blocks, and the phone on the kernel's command line.
- `common/devices/onyx.conf`, `common/devices/onyx.platform.c`: the phone's profile for the build.

## The patch

`onyx.patch` in the root of this repository is all of the above, against `8e61187`:

```sh
git apply --check onyx.patch    # in a clean checkout of hackios-xnu-kernel
git apply onyx.patch
```

It was checked with `git apply --check` against a clean worktree at `8e61187`, and the tree it makes is the same as
the one this work was done in (file for file, line endings apart).

A driver that needs a block the phone's device tree does not say where is not written to address zero: it says
which block is missing and does not use it. The blocks the phone does not have are the ones listed as 0 in the
table above.
