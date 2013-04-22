/*
 *  HID driver for the Xin-Mo Dual Arcade controller.
 *  Fixes the HID report descriptor by reporting the actual axis minimum that
 *  is sent, because hid-input in Linux > 3.x ignores out pf bounds values.
 *  (This module is based on "hid-saitek".)
 *
 *  Olivier Scherler
 *
 *  Copyright (c) 2012 Andreas Hübner
 */

/*
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 */

#include <linux/device.h>
#include <linux/hid.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include "hid-ids.h"

static __u8 *xinmo_report_fixup(struct hid_device *hdev, __u8 *rdesc,
		unsigned int *rsize)
{
	if (*rsize == 112 && rdesc[16] == 0x15 && rdesc[17] == 0xff
			&& rdesc[72] == 0x15 && rdesc[73] == 0xff) {

		hid_info(hdev, "Fixing up Xin-Mo Dual Arcade report descriptor\n");

		/* change Joystick 1 X and Y axes Logical Minimum to -2 */
		rdesc[17] = 0xfe;

		/* change Joystick 2 X and Y axes Logical Minimum to -2 */
		rdesc[73] = 0xfe;
	}
	return rdesc;
}

static const struct hid_device_id xinmo_devices[] = {
	{ HID_USB_DEVICE(USB_VENDOR_ID_XIN_MO, USB_DEVICE_ID_XIN_MO_DUAL_ARCADE)},
	{ }
};

MODULE_DEVICE_TABLE(hid, xinmo_devices);

static struct hid_driver xinmo_driver = {
	.name = "xinmo",
	.id_table = xinmo_devices,
	.report_fixup = xinmo_report_fixup
};

static int __init xinmo_init(void)
{
	return hid_register_driver(&xinmo_driver);
}

static void __exit xinmo_exit(void)
{
	hid_unregister_driver(&xinmo_driver);
}

module_init(xinmo_init);
module_exit(xinmo_exit);
MODULE_LICENSE("GPL");
