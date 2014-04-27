=============================
Android Blob Utility
by JackpotClavin
=============================

The Android Blob Utility is a program designed to make making AOSP-based ROMs easier to develop. What
is does is it allows the developer to pick proprietary file, type the file's absolute path, and this
program will spit back every library that should be needed in order to get that library or daemon to
run on an AOSP ROM.

How does it work? First the developer must dump their entire ROM's stock software to their computer, so
that typing the command "ls /home/android/dump/build.prop" will yield their device's build.prop. This is
just meant to ensure that you have the correct path. Secondly, the developer must download the Android
emulator that matches their device's stock software (if your stock /system dump is Android 4.3, download
the 4.3 emulator's image). Then extract the emulator's system.img into a directory and when the program
prompts you to enter the extracted emulator's directory, type the absolute path to it. Then enter however
many blobs you want this program to process. If you want to only process one blob, type 1 and hit enter.
After you enter the amount of blobs you want this program to process, you will type the absolute path to
the actual blob (see the example program usage below), and this program will print out all of the proprietary
blobs that are mentioned in that particular blob that you entered, that aren't in the emulator's /system dump.
That means that those files are either have to be built from source, or are proprietary and must be copied
into the ROM's build. This program will also format the blobs such that it is ready to be placed into a
vendor-blobs.mk file in your vendor folder of the ROM's source tree root. What makes this program great is
that it doesn't just search for the linked libraries that the blob wants, but it also searches for all of
the libraries that the blob wants that might satisfy the linker as it has all of the shared libraries, but
still not run properly as there are libraries that are opened in the actual code. This program also searches
recursively, so each and every blob that is found is also process through the searching algorithm, to see which
blobs *that* library also needs to run, so we cover all of the bases.

Example program usage:
JPC@ThinkPad-X220 ~ $ ./android-blob-utility
Emulator root??
/home/android/emulator
System dump root??
/home/android/dump
How many files?
1
Files to go: 1
File name??
/home/JPC/android/AndroidMisc/LGStock/bin/mm-qcamera-daemon
proprietary/vendor/lib/liboemcamera.so:system/vendor/lib/liboemcamera.so
proprietary/vendor/lib/libmmcamera2_stats_modules.so:system/vendor/lib/libmmcamera2_stats_modules.so
proprietary/vendor/lib/libmmcamera2_stats_algorithm.so:system/vendor/lib/libmmcamera2_stats_algorithm.so
proprietary/vendor/lib/libsensor1.so:system/vendor/lib/libsensor1.so
proprietary/vendor/lib/libqmi_encdec.so:system/vendor/lib/libqmi_encdec.so
proprietary/vendor/lib/libmmcamera2_iface_modules.so:system/vendor/lib/libmmcamera2_iface_modules.so
proprietary/vendor/lib/libmmcamera2_isp_modules.so:system/vendor/lib/libmmcamera2_isp_modules.so
proprietary/vendor/lib/libmmcamera_tintless_algo.so:system/vendor/lib/libmmcamera_tintless_algo.so
proprietary/vendor/lib/libmmcamera_tintless_bg_pca_algo.so:system/vendor/lib/libmmcamera_tintless_bg_pca_algo.so
proprietary/vendor/lib/libmmcamera2_sensor_modules.so:system/vendor/lib/libmmcamera2_sensor_modules.so
warning: library libmmcamera_%s.so missing or broken or a wildcard
proprietary/vendor/lib/libmmcamera2_pproc_modules.so:system/vendor/lib/libmmcamera2_pproc_modules.so
proprietary/vendor/lib/libmmcamera2_cpp_module.so:system/vendor/lib/libmmcamera2_cpp_module.so
proprietary/vendor/lib/libmmcamera2_c2d_module.so:system/vendor/lib/libmmcamera2_c2d_module.so
proprietary/vendor/lib/libC2D2.so:system/vendor/lib/libC2D2.so
proprietary/vendor/lib/libgsl.so:system/vendor/lib/libgsl.so
proprietary/vendor/lib/libc2d30.so:system/vendor/lib/libc2d30.so
proprietary/vendor/lib/libc2d30-a3xx.so:system/vendor/lib/libc2d30-a3xx.so
proprietary/vendor/lib/libc2d30-a4xx.so:system/vendor/lib/libc2d30-a4xx.so
proprietary/vendor/lib/libc2d2_z180.so:system/vendor/lib/libc2d2_z180.so
warning: library libc2d2.so missing or broken or a wildcard
proprietary/vendor/lib/libmmcamera2_imglib_modules.so:system/vendor/lib/libmmcamera2_imglib_modules.so
proprietary/vendor/lib/libmmcamera_imglib.so:system/vendor/lib/libmmcamera_imglib.so
proprietary/vendor/lib/libmmcamera_wavelet_lib.so:system/vendor/lib/libmmcamera_wavelet_lib.so
proprietary/vendor/lib/libadsprpc.so:system/vendor/lib/libadsprpc.so
warning: library lib%s_skel.so missing or broken or a wildcard
proprietary/vendor/lib/libmmcamera_hdr_gb_lib.so:system/vendor/lib/libmmcamera_hdr_gb_lib.so
proprietary/vendor/lib/libfastcvopt.so:system/vendor/lib/libfastcvopt.so
proprietary/vendor/lib/libOpenCL.so:system/vendor/lib/libOpenCL.so
proprietary/vendor/lib/libllvm-qcom.so:system/vendor/lib/libllvm-qcom.so
proprietary/vendor/lib/libCB.so:system/vendor/lib/libCB.so
proprietary/vendor/lib/egl/libGLESv2_adreno.so:system/vendor/lib/egl/libGLESv2_adreno.so
proprietary/vendor/lib/libadreno_utils.so:system/vendor/lib/libadreno_utils.so
proprietary/vendor/lib/egl/libq3dtools_adreno.so:system/vendor/lib/egl/libq3dtools_adreno.so
proprietary/vendor/lib/egl/libGLESv1_CM_adreno.so:system/vendor/lib/egl/libGLESv1_CM_adreno.so
proprietary/vendor/lib/egl/libEGL_adreno.so:system/vendor/lib/egl/libEGL_adreno.so
proprietary/vendor/lib/libOpenVG.so:system/vendor/lib/libOpenVG.so
proprietary/vendor/lib/egl/eglsubAndroid.so:system/vendor/lib/egl/eglsubAndroid.so
warning: library libGLESv2S3D_adreno.so missing or broken or a wildcard
proprietary/vendor/lib/libsc-a2xx.so:system/vendor/lib/libsc-a2xx.so
proprietary/vendor/lib/libsc-a3xx.so:system/vendor/lib/libsc-a3xx.so
proprietary/vendor/lib/libfastcvadsp_stub.so:system/vendor/lib/libfastcvadsp_stub.so
warning: library libfcvopt.so missing or broken or a wildcard
proprietary/vendor/lib/libmmcamera_faceproc.so:system/vendor/lib/libmmcamera_faceproc.so
warning: library libmmcamera_cac2_lib.so missing or broken or a wildcard
proprietary/vendor/lib/libmmcamera2_wnr_module.so:system/vendor/lib/libmmcamera2_wnr_module.so
proprietary/vendor/lib/libmmcamera2_vpe_module.so:system/vendor/lib/libmmcamera2_vpe_module.so
Completed sucessfully.

Note: see above where it says "libmmcamera_%s.so" as the output?
One of the above libraries runs through a list of libmmcamera_*.so libraries so each and every library
that begins with "libmmcamera_" should be copied into your ROM's build, and those libraries should
also be run through this program to see which libraries those "libmmcamera_*" need.
