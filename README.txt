=============================
Android Blob Utility
by JackpotClavin
=============================

The Android Blob Utility is a program designed to make developing AOSP-based
ROMs easier to develop. What is does is it allows the developer to pick
proprietary file, type the file's absolute path, and this program will spit
back every library that should be needed in order to get that library or daemon
to run on an AOSP ROM.

How does it work? First the developer must dump their entire stock ROM's
software to their computer, so that typing the command:
"ls /home/android/dump/build.prop" will yield their device's build.prop.
This is just meant to ensure that you have the correct path. Secondly, the
developer must edit the Makefile to match the SDK version of the stock system
dump's software (if your stock /system dump is Android 4.3, type in 18 (see
developer.android.com/guide/topics/manifest/uses-sdk-element.html#ApiLevels).
Next, you are to compile this program with the 'make' command. Once you run it,
this program will prompt you to enter however many blobs files you want this
program to process. If you want to only process one blob, type 1 and hit enter.
After you enter the amount of blobs you want this program to process, you will
type the absolute path to the actual blob (see the example program usage
below), and this program will print out all of the proprietary blobs that are
mentioned in that particular blob that you entered, that aren't in the
emulator's /system dump. That means that those files are either have to be
built from source, or are proprietary and must be copied into the ROM's build.
This program will also format the blobs such that it is ready to be placed into
a vendor-blobs.mk file in your vendor folder of the ROM's source tree root.

What makes this program great is that it doesn't just get the shared libraries
necessary to appease the linker, but it takes another step; it also catches the
blobs that may slip past the linker because they are called in the actual code
of the blob. Simply copying the shared libraries just appeases the linker, but
the blob still not run properly as the blob may want additional libraries.

This program also searches recursively, so each and every blob that is found is
also processed through the searching algorithm, to see which blobs *that*
library also needs to run, so we cover all of the bases in order to get a
proprietary library or daemon to run.

The following example was used on my LG G2. Running this program with the two
main proprietary files related to the camera (/system/bin/mm-qcamera-daemon and
/system/lib/hw/camera.msm8974.so), this program nicely printed out *every*
proprietary file needed to get the camera working, and formatted it so that one
can easily find and replace "proprietary/vendor/lib/..." with the name and
manufacturer of the device you wish to port AOSP-based ROMs to, instead of
having to keep pushing files until the linker is satisfied, or having to find
libraries that aren't shared libraries, but are called in the actual code of
the proprietary file.

Example program usage can be found in the Example_Usage.txt in this folder.

