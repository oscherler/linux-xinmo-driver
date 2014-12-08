# Patching the Linux Kernel to install the Xin-Mo Dual Arcade driver on Ubuntu 13.04

**Cliché Disclaimer:** Use these instructions at your own risks. Backup your computer before you start. I am not responsible for any data loss or fried computers caused by following these instructions.

## Ubuntu

I used Ubuntu 13.04, but it should be the same for the other affected versions (12.04, 12.10). Everything here happens in the terminal, make sure you’re logged in as an administrator (typically the user you created when you installed Ubuntu). Whenever a command that starts with `sudo` asks for your password, use the password for that administrator user. These instructions were derived from the [BuildYourOwnKernel][buildyourown] page on the [Ubuntu Wiki][ubuntu-wiki].

[buildyourown]: https://wiki.ubuntu.com/Kernel/BuildYourOwnKernel
[ubuntu-wiki]: https://wiki.ubuntu.com

### Observing the problem

First we’re going to install `evtest`, a tool to test the joystick:

	sudo apt-get update
	sudo apt-get install evtest

To try it out, connect your joystick and run the program:

	evtest

It will list devices and ask you to select your joystick. Find the line that says `Xin-Mo Xin-Mo Dual Arcade`, type the number next to `/dev/input/event` (`4` in this example) and Enter.

	No device specified, trying to scan all of /dev/input/event*
	Not running as root, no devices may be available.
	Available devices:
	/dev/input/event4:	Xin-Mo Xin-Mo Dual Arcade
	Select the device event number [0-5]: 4

Now when you move the joysticks and press buttons, it will print lines like these (for a button):

	Event: time 1378629038.490828, type 4 (EV_MSC), code 4 (MSC_SCAN), value 90001
	Event: time 1378629038.490828, type 1 (EV_KEY), code 288 (BTN_TRIGGER), value 1
	Event: time 1378629038.490828, -------------- SYN_REPORT ------------
	Event: time 1378629038.618674, type 4 (EV_MSC), code 4 (MSC_SCAN), value 90001
	Event: time 1378629038.618674, type 1 (EV_KEY), code 288 (BTN_TRIGGER), value 0
	Event: time 1378629038.618674, -------------- SYN_REPORT ------------

or these (for a joystick):

	Event: time 1378629075.546079, type 3 (EV_ABS), code 1 (ABS_Y), value 1
	Event: time 1378629075.546079, -------------- SYN_REPORT ------------
	Event: time 1378629075.740176, type 3 (EV_ABS), code 1 (ABS_Y), value 0
	Event: time 1378629075.740176, -------------- SYN_REPORT ------------

and nothing when you move the joysticks up or left.

### Fixing the problem

#### Downloading the sources

First we need to install the programs needed to build the kernel#[deviate-1]:

	sudo apt-get build-dep linux-image-$(uname -r)

[deviate-1]: And we already need to deviate from the instructions on the Ubuntu Wiki, because they tell you to download the sources first, but it will fail on a fresh install because we’re missing some tools. That shows us how carefully those instructions are written.

Then we can download the source code for the kernel:

	apt-get source linux-image-$(uname -r)

It will download and extract the following files to the current directory (the numbers can vary):

	linux-3.8.0
	linux_3.8.0-30.44.diff.gz
	linux_3.8.0-30.44.dsc
	linux_3.8.0.orig.tar.gz

#### Applying the patch

`linux-3.8.0` is the directory where the source code was extracted. Enter it:

	cd linux-3.8.0

Before we start, we need to install a tool called `fakeroot`#[deviate-2]:

	sudo apt-get install fakeroot

[deviate-2]: Second deviation, they forgot about installing `fakeroot`.

Then download [the patch][patch] using `wget`: 

	wget 'http://ithink.ch/blog/files/xin-mo/0001-hid-Add-new-driver-for-non-compliant-Xin-Mo-devices.patch'

[patch]: http://ithink.ch/blog/files/xin-mo/0001-hid-Add-new-driver-for-non-compliant-Xin-Mo-devices.patch

and apply it:

	patch -p1 < 0001-hid-Add-new-driver-for-non-compliant-Xin-Mo-devices.patch

It should display something like this:

	patching file drivers/hid/Kconfig
	Hunk #1 succeeded at 697 with fuzz 2 (offset -46 lines).
	patching file drivers/hid/Makefile
	Hunk #1 succeeded at 108 (offset -2 lines).
	patching file drivers/hid/hid-core.c
	Hunk #1 succeeded at 1738 (offset 2 lines).
	patching file drivers/hid/hid-ids.h
	Hunk #1 succeeded at 849 (offset -38 lines).
	patching file drivers/hid/hid-xinmo.c

#### Building the kernel

Run the following commands to start the build:

	fakeroot debian/rules clean
	fakeroot debian/rules binary-headers binary-generic

After a short while, it should pause to ask you about enabling a new driver:

	Xin-Mo non-fully compliant devices (HID_XINMO) [N/m/?] (NEW)

That’s the one we’re going through all this trouble for, so tell it to install the module by typing `m` and Enter.

Then the actual compilation will begin. It will take a while.

#### Installing the new kernel

Exit the `linux-3.8.0` directory:

	cd ..

Four debian packages were created (again, the numbers will vary):

	linux-headers-3.8.0-30_3.8.0-30.44_all.deb
	linux-headers-3.8.0-30-generic_3.8.0-30.44_amd64.deb
	linux-image-3.8.0-30-generic_3.8.0-30.44_amd64.deb
	linux-image-extra-3.8.0-30-generic_3.8.0-30.44_amd64.deb

Install them one by one:

	sudo dpkg -i linux-headers-3.8.0-30_3.8.0-30.44_all.deb
	sudo dpkg -i linux-headers-3.8.0-30-generic_3.8.0-30.44_amd64.deb
	sudo dpkg -i linux-image-3.8.0-30-generic_3.8.0-30.44_amd64.deb
	sudo dpkg -i linux-image-extra-3.8.0-30-generic_3.8.0-30.44_amd64.deb

And then reboot:

	sudo reboot

#### Test the patch

Run `evtest` again, like above, and enjoy the result:

	Event: time 1378634592.175998, type 3 (EV_ABS), code 1 (ABS_Y), value -1
	Event: time 1378634592.175998, -------------- SYN_REPORT ------------
	Event: time 1378634592.335920, type 3 (EV_ABS), code 1 (ABS_Y), value 0
	Event: time 1378634592.335920, -------------- SYN_REPORT ------------
	Event: time 1378634593.613888, type 3 (EV_ABS), code 0 (ABS_X), value -1
	Event: time 1378634593.613888, -------------- SYN_REPORT ------------
	Event: time 1378634593.809001, type 3 (EV_ABS), code 0 (ABS_X), value 0
	Event: time 1378634593.809001, -------------- SYN_REPORT ------------
	Event: time 1378634595.165686, type 3 (EV_ABS), code 3 (ABS_RX), value -1
	Event: time 1378634595.165686, -------------- SYN_REPORT ------------
	Event: time 1378634595.325688, type 3 (EV_ABS), code 3 (ABS_RX), value 0
	Event: time 1378634595.325688, -------------- SYN_REPORT ------------
	Event: time 1378634595.933826, type 3 (EV_ABS), code 2 (ABS_Z), value -1
	Event: time 1378634595.933826, -------------- SYN_REPORT ------------
	Event: time 1378634596.093610, type 3 (EV_ABS), code 2 (ABS_Z), value 0
	Event: time 1378634596.093610, -------------- SYN_REPORT ------------

If these instructions helped you, I would appreciate it if you could send me a couple of pictures of your set-up so I can compile them on a web page. Drop me a note [on Twitter](http://twitter.com/oscherler).
