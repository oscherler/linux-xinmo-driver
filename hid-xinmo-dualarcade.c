/*
 *  HID driver for the Xin-Mo Dual Arcade controller.
 *  Fixes the negative axix event values (-2) to match the HID report
 *  descriptor axis minimum (-1), because hid-input in Linux > 3.x ignores out
 *  of bounds values.
 *  (This module is based on "hid-saitek" and "hid-lg".)
 *
 *  Copyright (c) 2013 Olivier Scherler
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

/*
 * Fix negative events that are out of bounds.
 */
static int xinmo_event(struct hid_device *hdev, struct hid_field *field,
		struct hid_usage *usage, __s32 value)
{
	//struct xinmo_drv_data *drv_data = (struct lg_drv_data *)hid_get_drvdata(hdev);

	switch(usage->code) {
		case ABS_X:
		case ABS_Y:
		case ABS_Z:
		case ABS_RX:
			if(value < -1) {
				input_event(field->hidinput->input, usage->type, usage->code,
						-1);
				return 1;
			}
			break;
	}

	return 0;
}

static const struct hid_device_id xinmo_devices[] = {
	{ HID_USB_DEVICE(USB_VENDOR_ID_XIN_MO, USB_DEVICE_ID_XIN_MO_DUAL_ARCADE)},
	{ }
};

MODULE_DEVICE_TABLE(hid, xinmo_devices);

static struct hid_driver xinmo_driver = {
	.name = "xinmo",
	.id_table = xinmo_devices,
	//.report_fixup = xinmo_report_fixup
	.event = xinmo_event
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
