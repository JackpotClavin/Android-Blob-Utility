Android Blob Utility
=============================
by JackpotClavin

About
=============================

The Android Blob Utility is a program designed to make developing AOSP-based
ROMs easier. What it does is it allows the developer to pick
proprietary files, type in the files' absolute paths, and this program will spit
back every library that is needed in order to get that library or daemon
to run on an AOSP ROM.

How does it work?
=============================

First, the developer must dump their entire stock ROM's
software to their computer, so that typing the command:
`$ ls /path-to-build.prop/build.prop` will yield their device's build.prop. This is just meant to ensure that you have the
correct path. This dump can be can be obtained by doing `adb pull /system /path-to-destination/` with the device connected to
the computer, replacing `/path-to-destination/` to wherever you want to store the dump.  
Next, you are to compile this program with the `make` command. Once you run it,
the program will prompt you to enter whatever the SDK version of your /system
dump happens to be (check your /system/build.prop); for instance if your
/system dump is Android 4.3, type in `18` (use the following site for
[guidelines](https://developer.android.com/guide/topics/manifest/uses-sdk-element.html#ApiLevels)).  
Third, the program will ask you for the location of your /system dump; so if
the build.prop file is under `/home/android/dump/build.prop`, you can just type
`/home/android/dump` and press enter. The program will prompt you for the
manufacturer and device name. In the case of my Verizon LG G2, when
prompted for the manufacturer(vendor) and device name, I typed in `lge`, and
`vs980`, respectively.  
For the fourth step, getting the necessary blobs,
this program will prompt you to enter the amount of blob files you want this
program to process. If you want to only process one blob, type `1` and hit
enter. After you enter the amount of blobs you want this program to process, you
type the absolute path to the actual blob (see the [example program usage](https://github.com/JackpotClavin/Android-Blob-Utility/blob/master/Example_Usage.txt)),
and this program will print out all of the proprietary blobs that are mentioned
in that particular blob, that aren't in the emulator's /system
dump. This means that those files are either have to be built from source, or
are proprietary and must be copied into the ROM's build. This program will also
format the blobs so that it is ready to be placed into a vendor-blobs.mk file
in your vendor folder in the root of the ROM's source tree.

Features
==============================

What makes this program great is that it doesn't just get the shared libraries
necessary to appease the linker, but it goes another step, also catching the
blobs that may slip past the linker because they are only called in the actual code
of the blob. Simply copying the shared libraries just appeases the linker, but
the blob still may not run properly as the blob may want additional libraries.

This program also searches recursively, so each and every blob that is found is
also processed through the algorithm, to see which blobs *that*
library also needs to run, so we cover all of the bases in order to get a
proprietary library or daemon to run.

The following example was copied from LG G2. Running this program with the two
main proprietary files related to the camera `/system/bin/mm-qcamera-daemon` and
`/system/lib/hw/camera.msm8974.so`, this program nicely printed out *every*
proprietary file needed to get the camera working, and formatted it so that anyone
can easily copy the program's output to a vendor-blobs.mk file, instead of
having to continuously push files until the linker is satisfied, or having to find
libraries that aren't shared libraries, but are called in the actual code (of
the proprietary file).  
Example program usage can be found in the [Example_Usage.txt](https://github.com/JackpotClavin/Android-Blob-Utility/blob/master/Example_Usage.txt)
in this folder.

