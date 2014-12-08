# Patching the Linux Kernel to install the Xin-Mo Dual Arcade driver on a Raspberry Pi

**Cliché Disclaimer:** Use these instructions at your own risks. Backup your computer before you start. I am not responsible for any data loss or fried computers caused by following these instructions.

## Raspbian

I used [NOOBS 1.2.1][noobs] and selected Raspbian. Everything here happens in the terminal, make sure you’re logged in as an administrator (typically user `pi`). If a command that starts with `sudo` asks for your password (not normally the case on Raspbian), use the password for that administrator user. These instructions were derived from the [RPi Kernel Compilation][rpikernel] page on the [Embedded Linux Wiki][embedded-wiki].

[noobs]: http://www.raspberrypi.org/downloads	
[rpikernel]: http://elinux.org/RPi_Kernel_Compilation
[embedded-wiki]: http://elinux.org/Main_Page

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

First we need to install the programs needed to build the kernel:

	sudo apt-get update
	sudo apt-get -y dist-upgrade
	sudo apt-get -y install gcc make ncurses-dev

Then we can download the source code for the kernel. We’re going to use a ‘tarball’, the [rpi-3.6.y][] one, as proposed at the very end of the [Get the kernel source][get-source] section:

	wget 'https://github.com/raspberrypi/linux/archive/rpi-3.6.y.tar.gz'

[rpi-3.6.y]: https://github.com/raspberrypi/linux/archive/rpi-3.6.y.tar.gz
[get-source]: http://elinux.org/RPi_Kernel_Compilation#Get_the_kernel_source

Extract the sources:

	tar xpfvz rpi-3.6.y.tar.gz

You can now find the sources in the `linux-rpi-3.6.y` directory.

#### Applying the patch

`linux-rpi-3.6.y` is the directory where the source code was extracted. Enter it:

	cd linux-rpi-3.6.y

Then download [the patch][patch] using `wget`: 

	wget 'http://ithink.ch/blog/files/xin-mo/0001-hid-Add-new-driver-for-non-compliant-Xin-Mo-devices.patch'

[patch]: http://ithink.ch/blog/files/xin-mo/0001-hid-Add-new-driver-for-non-compliant-Xin-Mo-devices.patch

and apply it:

	patch -p1 < 0001-hid-Add-new-driver-for-non-compliant-Xin-Mo-devices.patch

It should display something like this:

	patching file drivers/hid/Kconfig
	Hunk #1 succeeded at 670 with fuzz 2 (offset -73 lines).
	patching file drivers/hid/Makefile
	Hunk #1 succeeded at 86 (offset -24 lines).
	patching file drivers/hid/hid-core.c
	Hunk #1 succeeded at 1689 (offset -47 lines).
	patching file drivers/hid/hid-ids.h
	Hunk #1 succeeded at 808 (offset -79 lines).
	patching file drivers/hid/hid-xinmo.c

#### Building the kernel

First, make sure the build directory is clean:

	make mrproper

Then copy the current kernel configuration from your Raspberry Pi:

	zcat /proc/config.gz > .config

We need to make the configuration up to date. This step might ask a lot of questions about whether to support some new features. We are going to take the default answer every time **except** for our Xin-Mo Dual Arcade driver. We want to make sure it’s enabled, since that’s the one we’re going through all this trouble for.

Run

	make oldconfig

and when the following question pops up, type m and Enter:

	Xin-Mo non-fully compliant devices (HID_XINMO) [N/m/y/?] (NEW)

For the other questions, if any, just type Enter to accept the default.

If you missed the `Xin-Mo` line or are not sure, you can use the menu config#[menuconfig] to make sure it is enabled. Type

	make menuconfig

and navigate to Device Drivers -> HID support -> Special HID drivers -> Xin-Mo non-fully compliant devices and type `m` to enable it as a module. An `<M>` should be displayed next to the line:

	<M> Xin-Mo non-fully compliant devices

Then select the Exit button until it asks if you want to save your nw configuration, and answer Yes.

[menuconfig]: **Note:** We installed the `ncurses-dev` package earlier required by `menuconfig`, but if you missed that step, you will get the following error:

		 *** Unable to find the ncurses libraries or the
		 *** required header files.
		 *** 'make menuconfig' requires the ncurses libraries.
		 ***
		 *** Install ncurses (ncurses-devel) and try again.
		 ***
	
	In this case, run
	
		sudo apt-get install ncurses-dev

When ready (ideally at the end of the evening), type

	make ; make modules

Then the actual compilation will begin. It will go on [for a long, long, long long, long, long time][long], so go to sleep.

[long]: http://www.youtube.com/watch?v=_2_YhqLl5-8

#### Installing the new kernel

Copy the new kernel image to the boot partition, under a name different than the existing one:

	sudo cp arch/arm/boot/Image /boot/kernel_new.img

Then enable it in the boot config by adding the following line to `/boot/config.txt`, or changing the `kernel=kernel.img` if it already exists:

	kernel=kernel_new.img

You can do this using the `nano` text editor:

	sudo nano /boot/config.txt

Type the `kernel=kernel_new.img` line on its own line or modify the existing one, then hit Control-O and the Enter to save, and Control-X to exit.

Now we need to install the modules:

	mkdir ~/modules
	export MODULES_TEMP=~/modules
	make INSTALL_MOD_PATH=${MODULES_TEMP} modules_install

and put the files into place:

	cd /lib

	sudo cp -R ~/modules/lib/firmware/ .
	sudo cp -R ~/modules/lib/modules/ .

Next the instructions recommend to “update your GPU firmware and libraries.” Since I didn’t spend a night recompiling the kernel to write this article, I cannot follow along as I write this, so if you started from a fresh and recent Raspbian installation, you can live dangerously and reboot (remember my disclaimer above, though). Otherwise, try and follow the instructions under [Get the firmware][getfirmware].

[getfirmware]: http://elinux.org/RPi_Kernel_Compilation#Get_the_firmware

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
