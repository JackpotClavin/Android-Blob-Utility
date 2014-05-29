/*
 * Android blob utility
 *
 * Copyright (C) 2014 JackpotClavin <jonclavin@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA  02110-1301, USA.
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

void dot_so_finder(char *filename);
void get_full_lib_name(char *found_lib);
void check_emulator_for_lib(char *check);
void mark_lib_as_processed(char *lib);
bool check_if_repeat(char *lib);
bool char_is_valid(char *s);

int num_blob_directories;
#define MAX_LIB_NAME 50
#define ALL_LIBS_SIZE 4096

//#define DEBUG

//#define DIRECTORIES_PROVIDED
#ifdef DIRECTORIES_PROVIDED
#include "system_directories.h"
#endif
/* The above flag should be enabled if you no-longer want to enter your emulator and source tree's
 * directory every single time this program is run. Simply enable the flag, and edit the header
 * file titled "system_directories.h" to point the emulator and system dump variables in the
 * correct location on your computer, and recompile.
 */
#define MAKE_VENDOR
/* The above MAKE_VENDOR flag will let this program print out the library's name in the form of:
 * proprietary/vendor/lib/libQSEEComAPI.so:system/vendor/lib/libQSEEComAPI.so
 * so it can easily be be 'find and replaced' to make a vendor-blobs.mk file for the vendor
 * folder of the AOSP source tree's root.
 */

const char *blob_directories[] = {
        "/vendor/lib/hw/",
        "/vendor/lib/egl/",
        "/vendor/lib/",
        "/vendor/bin/",
        "/lib/hw/",
        "/lib/egl/",
        "/lib/",
        "/bin/"
};

#ifdef DIRECTORIES_PROVIDED
const char emulator_root[128] = EMULATOR_ROOT;
const char system_dump_root[128] = SYSTEM_DUMP_ROOT;
#else
char emulator_root[128];
char system_dump_root[128];
#endif

char all_libs[ALL_LIBS_SIZE] = {0};

/* The purpose of this program is to help find proprietary libraries that are needed to
 * build AOSP-based ROMs. Running the top command on the stock ROM will help find proprietary
 * daemons that are started by the init*.rc scripts, and are normally-located in /system/bin/
 * For instance, Running this program on "/system/bin/mm-qcamera-daemon" will not-only find
 * the libraries needed that the linker tell you that you need, but also the libraries that
 * aren't listed as shared libraries at build time, which could slip past the linker but still
 * (probably) not allow the daemon to run correctly, or at all, and the worst part is because the
 * linker is satisfied, it won't display any error message about missing libraries. Since this
 * program mmaps the entire library and finds every instance of ".so" in libraries, it also will
 * recursively run the ".so" finder on *THOSE* libraries needed by the original file, so it
 * theoretically should spit back every single library needed to run the daemon, or a file such
 * as /system/lib/hw/camera.<board>.so will also work. The way to use this program is to
 * download a system.img of the Android emulator and extract it. You will also have to dump the
 * /system of your stock ROM, or extract a custom-recovery backup. When the program prompts you
 * for the emulator root, you are to type "/home/user/backup/emulator/system" so that if you were
 * to type the command "ls /home/user/backup/emulator/system/build.prop" it would yield the
 * emulator's build.prop file. Similarly, when it prompts your for the system dump root, you are
 * to type "/home/user/backup/dump/system" (without quotes, same for for emulator above) so that
 * typing the command "ls /home/user/backup/dump/system/build.prop" it would yield the dump's
 * build.prop. It will then analyze which proprietary files are missing from the emulator, thus,
 * you will have to put those files into the AOSP-based ROM to get the daemon/library to run.
 * Note: just because the library is present in the emulator doesn't mean you don't need the dump's
 * proprietary version to get the daemon to run! A proprietary /system/bin/rild might need its own
 * /system/lib/libril.so, even though this program will fail to mention libril.so, because it's in
 * the emulator!
 */

int main(int argc, char **argv) {

    char filename[128];
#ifndef DIRECTORIES_PROVIDED
    char buildprop_checker[128];
#endif
    char *found;
    char *last_slash;
    int num_files;
    char junk;

    num_blob_directories = sizeof(blob_directories) / sizeof(char*);
#ifndef DIRECTORIES_PROVIDED
    printf("Emulator root??\n");
    fgets(emulator_root, sizeof(emulator_root), stdin);
    found = strchr(emulator_root, '\n');
    if (found)
        *found = '\0';

    sprintf(buildprop_checker, "%s%s", emulator_root, "/build.prop");
    if (access(buildprop_checker, F_OK)) {
        printf("Error: build.prop file not found in emulator's root.\n");
        printf("Your path to the emulator is not correct.\n");
        printf("The command:\n");
        printf("\"ls %s%s\"\n", emulator_root, "/build.prop");
        printf("should yield the build.prop file.\n");
        printf("Exiting!\n");
        return 1;
    }

    printf("System dump root??\n");
    fgets(system_dump_root, sizeof(system_dump_root), stdin);
    found = strchr(system_dump_root, '\n');
    if (found)
        *found = '\0';

    sprintf(buildprop_checker, "%s%s", system_dump_root, "/build.prop");
    if (access(buildprop_checker, F_OK)) {
        printf("Error: build.prop file not found in system dump's root.\n");
        printf("Your path to the system dump is not correct.\n");
        printf("The command:\n");
        printf("\"ls %s%s\"\n", system_dump_root, "/build.prop");
        printf("should yield the build.prop file.\n");
        printf("Exiting!\n");
        return 1;
    }
#endif

    printf("How many files?\n");
    scanf("%d", &num_files);
    scanf("%c", &junk);
    while (num_files--) {
        printf("Files to go: %d\n", num_files + 1);
        printf("File name??\n");
        fgets(filename, sizeof(filename), stdin);
        found = strchr(filename, '\n');
        if (found)
            *found = '\0';

        dot_so_finder(filename);
        last_slash = strrchr(filename, '/');
        if (last_slash && !check_if_repeat(++last_slash))
        	check_emulator_for_lib(last_slash);
    }

    printf("Completed sucessfully.\n");

    return 0;
}

/* Purpose of this method is to open the library, my mmap-ing it, and traversing until it
 * until it finds ".so", (the ending of most Linux library names.) and then it will hand
 * it to the get_full_lib_name method. The "prepeek" pointer checks to make sure that the
 * character before the period in ".so" is a valid character (defined at the bottom of the
 * source) to cut down on false-positives where random binary-file junk just-so-happens to
 * have a random "][#$@#FW@&&.+^.so" laying around that doesn't pertain to a library, and
 * is just normal binary-file instructions and whatnot.
 */

void dot_so_finder(char *filename) {

    const char *lib_string = ".so";
    char *prepeek;

    int file_fd;

    void *file_map;
    char *ptr;
    struct stat file_stat;

    file_fd = open(filename, O_RDONLY);

    fstat(file_fd, &file_stat);

    file_map = mmap(0, file_stat.st_size, PROT_READ, MAP_PRIVATE, file_fd, 0);

    for (ptr = (char*)file_map; ptr < (char*)file_map + file_stat.st_size; ptr++) {
        prepeek = ptr - 1;
        if (!memcmp(ptr, lib_string, strlen(lib_string)) && char_is_valid(prepeek))
            get_full_lib_name(ptr);
    }
    close(file_fd);
}

/* After receiving a pointer to a location of memory that contains the string ".so" and
 * does not have a random bogus character before that which was filtered by the said
 * char_is_valid(prepeek), we now work our way backwards in memory to find find the string
 * "lib" or in rare cases "egl" (eglsubAndroid.so) and break out of the loop once we find
 * a match. We save the pointer to the period ".so", and add 3. Then we subtract that location
 * in memory from the instance of "lib" or "egl" so that value is the entire length of the lib
 * | lib_whatever.so | then strncpy the value into "full_name", and pass it to the check_emulator_for_lib
 * method which will search through the libraries directories of the emulator to see if there's
 * a library with that name that matches the one sent by get_full_lib_name. If it's missing, it means
 * that the library referenced is *not* in the emulator, which means:
 *
 * A. The file is a proprietary file, meaning it's needed by the service, and should be copied
 * into your vendor folder of your Android source tree, and referenced by your device source
 * tree with : $(call inherit-product-if-exists, vendor/manufacturer/codename/codename-vendor.mk)
 * (uncomment #define MAKE_VENDOR above to help automate this codename-vendor.mk) file.
 * (A quick word of notice is that just because the file is not in the emulator's system dump,
 * does not necessarily mean that the file is proprietary, it could just be that the emulator
 * does not need this file to be built, and is not built, so it may throw off this program. A
 * quick work around would be to type "mgrep lib_whatever" in your Android source tree's root,
 * and seeing if there are any hits in an Android.mk file. If there are, see B.
 *
 * B. It must be explicitly built and thus required in your device folder to be built such as
 * PRODUCT_PACKAGES += lib_whatever
 *
 * C. The library mentioned by the original file does not even exist in your device folder (it
 * should printf a message saying it's in neither the emulator not your system dump, this happens
 * occasionally)
 *
 * D. It's a wildcard, meaning the file searches for something like "libmmcamera_%s.so" so every
 * single library that starts with "libmmcamera_" should be copied into their respective directories
 * (/system/lib/ or /system/vendor/lib/) to appease the file in finding all its libraries mentioned
 *
 * E. The algorithm fucked up (sorry) in the worst possible case, something will segfault, and you
 * will see this by it either saying segfault, or the message at the bottom of the main method
 * "Completed sucessfully." will fail to appear.
 */

void get_full_lib_name(char *found_lib) {

    char *save;
    const char *lib = "lib";
    const char *egl = "egl";
    char *ptr;
    char *peek;
    char *second_peek;

    char full_name[128] = {0};

    long len;
    int num_chars;
    int i;

    ptr = found_lib;
    save = found_lib;
    peek = ptr - 1;

    /* if there's a false-positive in finding matching ".so", but it isn't ever referencing
     * a library, it's probably just instructions that slipped through the cracks. In this case
     * we will rewind the pointer that's searching for "lib" or "egl" MAX_LIB_NAME (default 50)
     * times, in which we will bail out citing that it was probably a false-positive
     */
    for (num_chars = 0; num_chars <= MAX_LIB_NAME; num_chars++) {
        if (!strncmp(ptr, egl, strlen(egl)) || !strncmp(ptr, lib, strlen(lib))) {
            second_peek = ptr - 1;
            /* the second peek below would fall victim to a file which is looking directly for
             * "/system/lib/lib_whatever.so", because it would now point to lib/lib_whatever.so
             * which is not what what we want, so take the first pick if the peek character is '/'
             */
            if (*second_peek == '/') {
            	for (i = 0; i < num_blob_directories; i++) {
            		if (!strncmp(second_peek, blob_directories[i], strlen(blob_directories[i]))) {
            			second_peek += strlen(blob_directories[i]);
            			ptr = second_peek;
            			break;
            		}
            	}
                break;
            }
            /* some libraries are called "libmmcamera_wavelet_lib.so", in which the pointer will
             * rewind to the first "lib" and then will pass it over to the check_emulator_for_lib
             * method, which will in turn bark about a missing "lib.so", so we will rewind the pointer
             * some extra times until it encounters a null character, or space (if it's mentioned in a
             * string) and if it ends up finding another instance of "lib", picks that *that* one, not
             * the original one, so we will get the entire library name of "libmmcamera_wavelet_lib.so"
             * and not just "lib.so" which would have been chosen if not for the second peek.
             */
            while (*second_peek-- && *second_peek != ' ') {
                if (!strncmp(second_peek, lib, strlen(lib))) {
#ifdef DEBUG
                    printf("Possible lib_lib.so!! %s\n", second_peek);
#endif
                    ptr = second_peek;
                }
            }
            break;
        }
        if (num_chars == MAX_LIB_NAME) {
#ifdef DEBUG
            /* This should print out bogus-characters/symbols if enabled */
            printf("skipping, too many characters, uncomment to show ptr's values!!!\n");
            printf("ptr is: %s\n", ptr);
#endif
            return;
        }
        ptr--;
        peek--;
    }
    len = (long)(save + 3) - (long)ptr;
    strncpy(full_name, ptr, len);
    if (check_if_repeat(full_name))
        return;

    check_emulator_for_lib(full_name);
}

/* I wrote this method when I was really tired and a bit sick so I apologize if the logic is
 * shit, but it works. We scan through the emulator's library directories and see if there's
 * a hit. If there is, we don't display anything. If there is no hit, we will print whether
 * it's missing in just the emulator, or if the referenced library is even in the original
 * source at all (sometimes obsolete things are never removed so they will give off a false-
 * positive)
 */

void check_emulator_for_lib(char *check) {

    char emulator_full_path[128];
    char system_dump_full_path[128];
#ifdef MAKE_VENDOR
    char vendor_string[512];
#endif
    int i;
    int missing = 0;
    bool found = false;

    mark_lib_as_processed(check); //mark the library as processed
    for (i = 0; i < num_blob_directories; i++) {
        sprintf(emulator_full_path, "%s%s%s", emulator_root, blob_directories[i], check);
        if (!access(emulator_full_path, F_OK)) {
            found = true;
        }
        if (i == (num_blob_directories - 1) && found == false) {
            for (i = 0; i < num_blob_directories; i++) {
                sprintf(system_dump_full_path, "%s%s%s", system_dump_root,
                        blob_directories[i], check);
                if (access(system_dump_full_path, F_OK)) {
                    //printf("missing: %s", system_dump_full_path);
                    missing++;
                }
            }
            if (missing == num_blob_directories)
                printf("warning: blob file %s missing or broken or a wildcard\n", check);

            for (i = 0; i < num_blob_directories; i++) {
                sprintf(system_dump_full_path, "%s%s%s", system_dump_root,
                        blob_directories[i], check);
                if (!access(system_dump_full_path, F_OK)) {
                    sprintf(system_dump_full_path, "%s%s%s", system_dump_root,
                            blob_directories[i], check);
#ifndef MAKE_VENDOR
                    printf("blob file %s not found in emulator!!!!\n", system_dump_full_path);
#else
                    sprintf(vendor_string, "%s%s%s%c%s%s%s", "proprietary", blob_directories[i], check,
                            ':', "system", blob_directories[i], check);
                    printf("%s\n", vendor_string);
#endif
                    dot_so_finder(system_dump_full_path);
                }
            }
        }
    }
}

/* Check to see if the characters normally appear in the name of libraries, and not an instruction
 * which might look like &^%@.so
 */

bool char_is_valid(char *s) {
    if (*s >= 'a' && *s <= 'z')
        return true;
    if (*s >= 'A' && *s <= 'Z')
        return true;
    if (*s >= '0' && *s <= '9')
        return true;
    if (*s == '_' || *s == '-')
        return true;
    if (*s == 0)
        return true;
    if (*s == '%') /* wildcard, bitches! */
        return true;
    return false;
}

/* No need to print out libdiag.so 100 times, so if it's the first time, add it to the list
 * of libraries that we have found that are missing and be done with it.
 */

bool check_if_repeat(char *lib) {
    int i;
    for (i = 0; i < ALL_LIBS_SIZE; i++) {
        if (!memcmp(all_libs + i, lib, strlen(lib))) {
            //printf("skipping %s!!\n", lib);
            return true;
        }
    }
    return false;
}

/* If it's the first time a library is found, add it do the repository of libraries that
 * have been mentioned. There is no need to keep spitting out the same library 100 times
 * if it's needed by multiple libraries.
 */

void mark_lib_as_processed(char *lib) {
    static int offset = 0;
    while (*lib) {
        all_libs[offset] = *lib;
        offset++;
        lib++;
    }
    offset++;
#ifdef DEBUG
    printf("%d\n", offset);
#endif
}
