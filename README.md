# Linux Xin-Mo Driver

Linux HID driver for Xin-Mo devices. Included in the Linux kernel starting from version 3.12 (Nov. 2013).

Currently, only the Xin-Mo Dual Arcade Controller is supported.

## Introduction

The [Xin-Mo Dual Arcade Controller][xinmo] is a USB device that allows to connect arcade-style controls like buttons and joysticks to a computer.

Unfortunately, it did not work properly on Linux, because the negative values sent by the joysticks are out of range compared to what is declared in the HID descriptor. Starting with version 3.3 of the kernel, the HID driver discards out of range values.

This driver fixes the negative axis event values sent by the Dual Arcade (`-2`) to match the logical axis minimum of the HID report descriptor (`-1`).

[xinmo]: http://www.xin-mo.com/?page_id=34

## History

The issue was first reported by [stealth][] in [Ubuntu bug #1077359][bug] and the cause was identified by [Lokagan][].

This code was merged in version 3.12 of the Linux kernel, released on Nov 4, 2013, and thus is available in Ubuntu starting from version 14.04 LTS, and in other distributions starting from `${VERSION_THAT_INCLUDES_KERNEL_GTE_3_12}`.

[bug]: https://bugs.launchpad.net/bugs/1077359
[stealth]: https://launchpad.net/~stealth
[Lokagan]: https://launchpad.net/~lokagan

## Contributors

**Reporter:** [stealth][]  
**Investigator:** [Lokagan][]  
**Author:** Olivier Scherler [olbaum]  
**Fixes:** Wei Yongjun and Brian Norris

[olbaum]: https://github.com/oscherler/
