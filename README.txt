POCO F7 (onyx) port of HackiOS -- what is in this zip
=====================================================

This is the POCO F7 (onyx, Snapdragon 8 Elite / SM8735) port of the HackiOS XNU
kernel. It is a work in progress: the kernel's addresses come from the phone's
own device tree, but it has never been on the phone.

The upstream repository is https://github.com/oosexgang/hackios-xnu-kernel and
the patch is against commit 8e61187 ("Add LICENSE").

What is here
------------

onyx.patch
    The whole port as one patch: 21 files, +1125/-145. It was checked with
    `git apply --check` against a clean worktree at 8e61187, and the tree it
    makes is file-for-file the same as the tree it was made in.

changed-files/
    The same 21 files, in the repository's own layout, for copying over a
    checkout by hand instead of applying the patch.

docs/PORTING-onyx.md
    What is done, what is not, the whole address table with the device tree
    node each address came from, and what has to be read off the phone
    (the framebuffer base, the UEFI memory map, the PMIC arbiter, the shared
    memory's lock, the SoundWire master, the a7xx GPU registers).

How to use it
-------------

With the patch (recommended):

    git clone https://github.com/oosexgang/hackios-xnu-kernel
    cd hackios-xnu-kernel
    git checkout 8e61187
    git apply --check ../onyx.patch
    git apply ../onyx.patch

Or copy the files over the checkout (they are in the repository's layout
under changed-files/).

What the port does
------------------

The kernel serves more than one phone, so an address cannot be compiled into
it. The loader says which phone it is bringing up on the kernel's command
line ("soc=onyx"), and the drivers read the addresses of the phone's parts
from a profile (xnu/files/bsd/dev/redwood_soc.c). The POCO X5 Pro 5G
(redwood) addresses are in the profile too, unchanged: nothing changes for
the phone that already works.

A driver that needs a block the phone's device tree does not say where is
not written to address zero: it says which block is missing and does not use
it. On this phone that is the touch screen (a FocalTech behind Trusted
Touch), the GPU (an Adreno a7xx; the driver knows the a6xx of the Snapdragon
778G), the PMIC arbiter (the buttons, the torch and the cameras) and the
SoundWire master (the microphone).

What is still missing, and needs the phone
------------------------------------------

- the framebuffer base the UEFI reports: the loader picks the phone by it,
  and without it the loader falls back to QEMU's description
- the UEFI memory map (uefi/files/Platforms/Xiaomi/onyxPkg): it cannot be
  copied from redwood, the Snapdragon 8 Elite's memory is laid out differently
- the PMIC arbiter's, the shared memory's lock's and the SoundWire master's
  addresses: they are in the vendor's own device tree, not in the public one
- the a7xx GPU's registers, for a driver of its own
- the phone's own firmware (the WCN786x radio, the PM7550BA audio, the GPU)

See docs/PORTING-onyx.md for the details of each.
