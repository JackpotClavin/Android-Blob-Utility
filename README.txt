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
developer must download the Android emulator that matches their device's stock
software (if your stock /system dump is Android 4.3, download the 4.3
emulator's image). Then extract the emulator's system.img into a directory.
Then when this program prompts you to enter the extracted emulator's directory,
type the absolute path to it. Then enter however many blobs you want this
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

Example program usage:
JPC@ThinkPad-X220 ~ $ ./android-blob-utility
Emulator root??
/home/android/emulator
System dump root??
/home/android/dump
How many files?
2
Files to go: 2
File name??
/home/android/dump/bin/mm-qcamera-daemon
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
proprietary/vendor/lib/libmmcamera_imx135_eeprom.so:system/vendor/lib/libmmcamera_imx135_eeprom.so
proprietary/vendor/lib/libmmcamera_imx135.so:system/vendor/lib/libmmcamera_imx135.so
proprietary/vendor/lib/libchromatix_imx135_common.so:system/vendor/lib/libchromatix_imx135_common.so
proprietary/vendor/lib/libchromatix_imx135_snapshot.so:system/vendor/lib/libchromatix_imx135_snapshot.so
proprietary/vendor/lib/libchromatix_imx135_default_video.so:system/vendor/lib/libchromatix_imx135_default_video.so
proprietary/lib/libchromatix_imx135_liveshot.so:system/lib/libchromatix_imx135_liveshot.so
proprietary/vendor/lib/libchromatix_imx135_preview.so:system/vendor/lib/libchromatix_imx135_preview.so
proprietary/vendor/lib/libchromatix_imx135_video_qtr.so:system/vendor/lib/libchromatix_imx135_video_qtr.so
proprietary/vendor/lib/libchromatix_imx135_video_dualrec.so:system/vendor/lib/libchromatix_imx135_video_dualrec.so
proprietary/vendor/lib/libchromatix_imx135_hfr_60.so:system/vendor/lib/libchromatix_imx135_hfr_60.so
proprietary/vendor/lib/libchromatix_imx135_mms_video.so:system/vendor/lib/libchromatix_imx135_mms_video.so
proprietary/vendor/lib/libchromatix_imx135_uhd_video.so:system/vendor/lib/libchromatix_imx135_uhd_video.so
proprietary/vendor/lib/libchromatix_imx135_hfr_120.so:system/vendor/lib/libchromatix_imx135_hfr_120.so
proprietary/vendor/lib/libchromatix_imx135_video_hdr.so:system/vendor/lib/libchromatix_imx135_video_hdr.so
proprietary/vendor/lib/libchromatix_imx135_fuji_common.so:system/vendor/lib/libchromatix_imx135_fuji_common.so
proprietary/vendor/lib/libchromatix_imx135_fuji_snapshot.so:system/vendor/lib/libchromatix_imx135_fuji_snapshot.so
proprietary/vendor/lib/libchromatix_imx135_fuji_default_video.so:system/vendor/lib/libchromatix_imx135_fuji_default_video.so
warning: blob file libchromatix_imx135_fuji_liveshot.so missing or broken
proprietary/vendor/lib/libchromatix_imx135_fuji_preview.so:system/vendor/lib/libchromatix_imx135_fuji_preview.so
proprietary/vendor/lib/libchromatix_imx135_fuji_video_qtr.so:system/vendor/lib/libchromatix_imx135_fuji_video_qtr.so
proprietary/vendor/lib/libchromatix_imx135_fuji_video_dualrec.so:system/vendor/lib/libchromatix_imx135_fuji_video_dualrec.so
proprietary/vendor/lib/libchromatix_imx135_fuji_hfr_60.so:system/vendor/lib/libchromatix_imx135_fuji_hfr_60.so
proprietary/vendor/lib/libchromatix_imx135_fuji_mms_video.so:system/vendor/lib/libchromatix_imx135_fuji_mms_video.so
warning: blob file libchromatix_imx135_fuji_uhd_video.so missing or broken
proprietary/vendor/lib/libchromatix_imx135_fuji_hfr_120.so:system/vendor/lib/libchromatix_imx135_fuji_hfr_120.so
warning: blob file libchromatix_imx135_fuji_video_hdr.so missing or broken
proprietary/vendor/lib/libmmcamera_hdr_gb_lib.so:system/vendor/lib/libmmcamera_hdr_gb_lib.so
proprietary/vendor/lib/libfastcvopt.so:system/vendor/lib/libfastcvopt.so
proprietary/vendor/lib/libOpenCL.so:system/vendor/lib/libOpenCL.so
proprietary/vendor/lib/libllvm-qcom.so:system/vendor/lib/libllvm-qcom.so
proprietary/vendor/lib/libgsl.so:system/vendor/lib/libgsl.so
proprietary/vendor/lib/libCB.so:system/vendor/lib/libCB.so
proprietary/vendor/lib/egl/libGLESv2_adreno.so:system/vendor/lib/egl/libGLESv2_adreno.so
proprietary/vendor/lib/libadreno_utils.so:system/vendor/lib/libadreno_utils.so
proprietary/vendor/lib/egl/libq3dtools_adreno.so:system/vendor/lib/egl/libq3dtools_adreno.so
proprietary/vendor/lib/egl/libGLESv1_CM_adreno.so:system/vendor/lib/egl/libGLESv1_CM_adreno.so
proprietary/vendor/lib/egl/libEGL_adreno.so:system/vendor/lib/egl/libEGL_adreno.so
proprietary/vendor/lib/libOpenVG.so:system/vendor/lib/libOpenVG.so
proprietary/vendor/lib/egl/eglsubAndroid.so:system/vendor/lib/egl/eglsubAndroid.so
warning: blob file libGLESv2S3D_adreno.so missing or broken
proprietary/vendor/lib/libsc-a2xx.so:system/vendor/lib/libsc-a2xx.so
proprietary/vendor/lib/libsc-a3xx.so:system/vendor/lib/libsc-a3xx.so
proprietary/vendor/lib/libadsprpc.so:system/vendor/lib/libadsprpc.so
proprietary/vendor/lib/libfastcvadsp_stub.so:system/vendor/lib/libfastcvadsp_stub.so
warning: blob file libfcvopt.so missing or broken
proprietary/vendor/lib/libmmcamera_hdr_lib.so:system/vendor/lib/libmmcamera_hdr_lib.so
proprietary/vendor/lib/libmmcamera_imglib.so:system/vendor/lib/libmmcamera_imglib.so
proprietary/vendor/lib/libmmcamera_wavelet_lib.so:system/vendor/lib/libmmcamera_wavelet_lib.so
proprietary/vendor/lib/libmmcamera_faceproc.so:system/vendor/lib/libmmcamera_faceproc.so
warning: blob file libmmcamera_cac2_lib.so missing or broken
proprietary/vendor/lib/libmmcamera_imx132.so:system/vendor/lib/libmmcamera_imx132.so
proprietary/vendor/lib/libchromatix_imx132_common.so:system/vendor/lib/libchromatix_imx132_common.so
proprietary/vendor/lib/libchromatix_imx132_preview.so:system/vendor/lib/libchromatix_imx132_preview.so
proprietary/vendor/lib/libchromatix_imx132_default_video.so:system/vendor/lib/libchromatix_imx132_default_video.so
proprietary/vendor/lib/libchromatix_imx132_vt.so:system/vendor/lib/libchromatix_imx132_vt.so
proprietary/vendor/lib/libchromatix_imx132_mms_video.so:system/vendor/lib/libchromatix_imx132_mms_video.so
proprietary/vendor/lib/libmmcamera_tuning.so:system/vendor/lib/libmmcamera_tuning.so
proprietary/vendor/lib/libmmcamera2_pproc_modules.so:system/vendor/lib/libmmcamera2_pproc_modules.so
proprietary/vendor/lib/libmmcamera2_cpp_module.so:system/vendor/lib/libmmcamera2_cpp_module.so
proprietary/vendor/lib/libmmcamera2_c2d_module.so:system/vendor/lib/libmmcamera2_c2d_module.so
proprietary/vendor/lib/libC2D2.so:system/vendor/lib/libC2D2.so
proprietary/vendor/lib/libc2d30.so:system/vendor/lib/libc2d30.so
proprietary/vendor/lib/libc2d30-a3xx.so:system/vendor/lib/libc2d30-a3xx.so
proprietary/vendor/lib/libc2d30-a4xx.so:system/vendor/lib/libc2d30-a4xx.so
proprietary/vendor/lib/libc2d2_z180.so:system/vendor/lib/libc2d2_z180.so
warning: blob file libc2d2.so missing or broken
proprietary/vendor/lib/libmmcamera2_imglib_modules.so:system/vendor/lib/libmmcamera2_imglib_modules.so
proprietary/vendor/lib/libmmcamera2_wnr_module.so:system/vendor/lib/libmmcamera2_wnr_module.so
proprietary/vendor/lib/libmmcamera2_vpe_module.so:system/vendor/lib/libmmcamera2_vpe_module.so
proprietary/lib/libmm-qcamera.so:system/lib/libmm-qcamera.so
proprietary/lib/libmmcamera_interface.so:system/lib/libmmcamera_interface.so
proprietary/lib/libHDR.so:system/lib/libHDR.so
proprietary/lib/libmorpho_noise_reduction.so:system/lib/libmorpho_noise_reduction.so
proprietary/lib/libmorpho_image_stab31.so:system/lib/libmorpho_image_stab31.so
proprietary/lib/libmorpho_movie_stabilization.so:system/lib/libmorpho_movie_stabilization.so
proprietary/lib/libmorpho_video_denoiser.so:system/lib/libmorpho_video_denoiser.so
proprietary/lib/libmmjpeg_interface.so:system/lib/libmmjpeg_interface.so
proprietary/lib/libqomx_core.so:system/lib/libqomx_core.so
proprietary/vendor/lib/libqomx_jpegenc.so:system/vendor/lib/libqomx_jpegenc.so
proprietary/vendor/lib/libmmqjpeg_codec.so:system/vendor/lib/libmmqjpeg_codec.so
proprietary/vendor/lib/libmmjpeg.so:system/vendor/lib/libmmjpeg.so
proprietary/vendor/lib/libjpegehw.so:system/vendor/lib/libjpegehw.so
proprietary/vendor/lib/libjpegdhw.so:system/vendor/lib/libjpegdhw.so
proprietary/vendor/lib/libqomx_jpegdec.so:system/vendor/lib/libqomx_jpegdec.so
proprietary/bin/mm-qcamera-daemon:system/bin/mm-qcamera-daemon
Files to go: 1
File name??
/home/android/dump/lib/hw/camera.msm8974.so
proprietary/lib/libVDObjectTrackerAPI.so:system/lib/libVDObjectTrackerAPI.so
proprietary/vendor/lib/libthermalclient.so:system/vendor/lib/libthermalclient.so
proprietary/vendor/lib/libdiag.so:system/vendor/lib/libdiag.so
proprietary/lib/hw/camera.msm8974.so:system/lib/hw/camera.msm8974.so
Completed sucessfully.

