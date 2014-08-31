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
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#ifdef USE_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

void dot_so_finder(char *filename);
void get_full_lib_name(char *found_lib);
void check_emulator_for_lib(char *emulator_check);
void mark_lib_as_processed(char *lib);
bool check_if_repeat(char *lib);
bool char_is_valid(char *s);
void process_wildcard(char *wildcard);
void find_wildcard_libraries(char *beginning, char *end);
bool build_prop_checker(void);
void get_lib_from_system_dump(char *system_check);
bool check_emulator_files_for_match(char *emulator_full_path);
void strip_newline(char *line);

int num_blob_directories;
#define MAX_LIB_NAME 50
#define ALL_LIBS_SIZE 16384 //16KB

//#define DEBUG

//#define VARIABLES_PROVIDED
#ifdef VARIABLES_PROVIDED
#include "system_directories.h"
#endif
/* The above flag should be enabled if you no-longer want to enter your device's source tree's
 * directory every single time this program is run. Simply enable the flag, and edit the header
 * file titled "system_directories.h" to point your device's system dump directory in the
 * correct location on your computer, and recompile.
 */

const char *blob_directories[] = {
        "/vendor/lib/egl/",
        "/vendor/lib/hw/",
        "/vendor/lib/",
        "/vendor/bin/",
        "/lib/egl/",
        "/lib/hw/",
        "/lib/",
        "/bin/"
};

#ifdef VARIABLES_PROVIDED
#ifndef USE_READLINE
const char system_dump_root[128] = SYSTEM_DUMP_ROOT;
#else
const char *system_dump_root = SYSTEM_DUMP_ROOT;
#endif /* USE_READLINE */
#else
#ifndef USE_READLINE
char system_dump_root[128];
#else
char *system_dump_root;
#endif /* USE_READLINE */
#endif

char all_libs[ALL_LIBS_SIZE] = {0};
char *sdk_buffer;

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
 * as /system/lib/hw/camera.<board>.so will also work. The way to use this program is to dump the
 * /system of your stock ROM, or extract a custom-recovery backup. When the program prompts you
 * for the system dump root, you type "/home/user/backup/dump/system" (without quotes) so that
 * typing the command "ls /home/user/backup/dump/system/build.prop" it would yield the dump's
 * build.prop. It will then analyze which proprietary files are missing from the emulator, thus,
 * you will have to put those files into the AOSP-based ROM to get the daemon/library to run.
 * Note: just because the library is present in the emulator doesn't mean you don't need the dump's
 * proprietary version to get the daemon to run! A proprietary /system/bin/rild might need its own
 * /system/lib/libril.so, even though this program will fail to mention libril.so, because it's in
 * the emulator!
 */

int main(int argc, char **argv) {

    char *last_slash;
    char emulator_system_file[32];
    int num_files;
    int sdk_version;
#ifdef VARIABLES_PROVIDED
    sdk_version = SYSTEM_DUMP_SDK_VERSION;
#endif
    long length = 0;
    FILE *fp;

#ifndef USE_READLINE
    char filename[128];
#else
    char *filename;
#endif

    num_blob_directories = sizeof(blob_directories) / sizeof(char*);

#ifndef VARIABLES_PROVIDED
    printf("System dump SDK version?\n");
    printf("See: https://developer.android.com/guide/topics/manifest/uses-sdk-element.html#ApiLevels\n");
    scanf("%d%*c", &sdk_version);
#endif

    sprintf(emulator_system_file, "emulator_systems/sdk_%d.txt", sdk_version);
    fp = fopen(emulator_system_file, "r");
    if (!fp) {
        printf("SDK text file %s not found, exiting!\n", emulator_system_file);
        return 1;
    }
    fseek(fp, 0, SEEK_END);
    length = ftell(fp);
    rewind(fp);

    sdk_buffer = (char*)malloc(sizeof(char) * length);
    fread(sdk_buffer, 1, length, fp);
    fclose(fp);

#ifndef VARIABLES_PROVIDED
#ifndef USE_READLINE
    printf("System dump root?\n");
    fgets(system_dump_root, sizeof(system_dump_root), stdin);
#else
    system_dump_root = readline("System dump root?\n");
#endif
    strip_newline(system_dump_root);

    if (build_prop_checker())
        return 1;
#endif

    printf("How many files?\n");
    scanf("%d%*c", &num_files);

    while (num_files--) {
        printf("Files to go: %d\n", num_files + 1);
#ifndef USE_READLINE
        printf("File name?\n");
        fgets(filename, sizeof(filename), stdin);
#else
        filename = readline("File name?\n");
#endif
        strip_newline(filename);

        dot_so_finder(filename);
        last_slash = strrchr(filename, '/');
        if (last_slash)
            check_emulator_for_lib(++last_slash);
    }

    printf("Completed successfully.\n");
    free(sdk_buffer);

    return 0;
}

/* Purpose of this method is to open the library, by mmap-ing it, and traversing until it
 * until it finds ".so", (the ending of most Linux library names.) and then it will hand
 * it to the get_full_lib_name method. The "prepeek" pointer checks to make sure that the
 * character before the period in ".so" is a valid character (defined at the bottom of the
 * source) to cut down on false-positives where random binary-file junk just-so-happens to
 * have a random "][#$@#FW@&&.+^.so" laying around that doesn't pertain to a library, and
 * is just normal binary-file instructions and whatnot.
 */

void dot_so_finder(char *filename) {

    const char *lib_string = ".so";

    int file_fd;

    void *file_map;
    char *ptr;
    struct stat file_stat;

    file_fd = open(filename, O_RDONLY);

    fstat(file_fd, &file_stat);

    file_map = mmap(0, file_stat.st_size, PROT_READ, MAP_PRIVATE, file_fd, 0);

    for (ptr = (char*)file_map; ptr < (char*)file_map + file_stat.st_size; ptr++) {
        if (!memcmp(ptr, lib_string, 3) && char_is_valid(ptr - 1))
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
 * D. The algorithm fucked up (sorry) in the worst possible case, something will segfault, and you
 * will see this by it either saying segfault, or the message at the bottom of the main method
 * "Completed successfully." will fail to appear.
 */

void get_full_lib_name(char *found_lib) {

    const char *lib = "lib";
    const char *egl = "egl";
    char *ptr, *peek;

    char full_name[128] = {0};

    long len;
    int num_chars;
    int i;

    ptr = found_lib;
    peek = ptr - 1;

    /* if there's a false-positive in finding matching ".so", but it isn't ever referencing
     * a library, it's probably just instructions that slipped through the cracks. In this case
     * we will rewind the pointer that's searching for "lib" or "egl" MAX_LIB_NAME (default 50)
     * times, in which we will bail out citing that it was probably a false-positive
     */
    for (num_chars = 0; num_chars <= MAX_LIB_NAME; num_chars++) {
        if (!strncmp(ptr, egl, 3) || !strncmp(ptr, lib, 3)) {
            peek = ptr - 1;
            /* the peek below would fall victim to a file which is looking directly for
             * "/system/lib/lib_whatever.so", because it would now point to lib/lib_whatever.so
             * which is not what what we want, so take the first pick if the peek character is '/'
             */
            if (*peek == '/') {
                for (i = 0; i < num_blob_directories; i++) {
                    if (!strncmp(peek, blob_directories[i], strlen(blob_directories[i]))) {
                        peek += strlen(blob_directories[i]);
                        ptr = peek;
                        break;
                    }
                }
                break;
            }
            /* some libraries are called "libmmcamera_wavelet_lib.so", in which the pointer will
             * rewind to the first "lib" and then will pass it over to the check_emulator_for_lib
             * method, which will in turn bark about a missing "lib.so", so we will rewind the pointer
             * some extra times until it encounters an invalid character using the char_is_valid
             * function and if it ends up finding another instance of "lib", picks that *that* one, not
             * the original one, so we will get the entire library name of "libmmcamera_wavelet_lib.so"
             * and not just "lib.so" which would have been chosen if not for the peek.
             */
            while (char_is_valid(peek) && *peek--) {
                if (!strncmp(peek, lib, 3)) {
#ifdef DEBUG
                    printf("Possible lib_lib.so!! %s\n", peek);
#endif
                    ptr = peek;
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
    len = (long)(found_lib + 3) - (long)ptr;
    strncpy(full_name, ptr, len);

    check_emulator_for_lib(full_name);
}

/* We scan through the emulator's library directories and see if there's a hit. If there is,
 * we don't display anything. If there is no hit, we hand it over to the function called
 * get_lib_from_system_dump.
 */

void check_emulator_for_lib(char *emulator_check) {

    char emulator_full_path[128];
    int i;

    if (check_if_repeat(emulator_check))
        return;

    mark_lib_as_processed(emulator_check); //mark the library as processed

    for (i = 0; i < num_blob_directories; i++) {
        sprintf(emulator_full_path, "/system%s%s", blob_directories[i], emulator_check);
        /* don't do anything if the file is in the emulator, as that means it's not proprietary. */
        if (check_emulator_files_for_match(emulator_full_path))
            return;
    }

    /* if we've made it this far, the blob is NOT in the emulator so that means it is proprietary
     * or an obsolete reference to a blob that is not even in the system dump.
     */
    get_lib_from_system_dump(emulator_check);
}

bool check_emulator_files_for_match(char *emulator_full_path) {
    return strstr(sdk_buffer, emulator_full_path);
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
    int lib_length = strlen(lib);

    for (i = 0; i < ALL_LIBS_SIZE; i++) {
        if (!memcmp(all_libs + i, lib, lib_length)) {
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
    if (offset > ALL_LIBS_SIZE - 100)
        printf("You may need to increase the ALL_LIBS_SIZE macro.\n");
#ifdef DEBUG
    printf("%d\n", offset);
#endif
}

void process_wildcard(char *wildcard) {

    char *ptr;
    char beginning[16] = {0};
    char end[16] = {0};

    ptr = strstr(wildcard, "%");
    if (ptr) {
        strncpy(beginning, wildcard, ptr - wildcard);
        ptr += 2; //advance beyond the format specifier (normally %s or possibly %c)
        strcpy(end, ptr);
    }

    find_wildcard_libraries(beginning, end);
}

void find_wildcard_libraries(char *beginning, char *end) {

    DIR *dir;
    struct dirent *dirent;
    char full_path[128] = {0};
    int i;
    bool found = false;

    for (i = 0; i < num_blob_directories; i++) {
        sprintf(full_path, "%s%s", system_dump_root, blob_directories[i]);
        dir = opendir(full_path);
        if (!dir)
            continue;

        while ((dirent = readdir(dir)) != NULL) {
            if (strstr(dirent->d_name, beginning) && strstr(dirent->d_name, end)) {
                check_emulator_for_lib(dirent->d_name);
                found = true;
            }
        }
        closedir(dir);
    }

    if (!found)
        printf("warning: wildcard %s%s%s missing or broken\n", beginning, "%s", end);
}

bool build_prop_checker(void) {

    char buildprop_checker[128];

    sprintf(buildprop_checker, "%s/build.prop", system_dump_root);
    if (access(buildprop_checker, F_OK)) {
        printf("Error: build.prop file not found in system dump's root.\n");
        printf("Your path to the system dump is not correct.\n");
        printf("The command:\n");
        printf("\"ls %s/build.prop\"\n", system_dump_root);
        printf("should yield the system dump's build.prop file.\n");
        printf("Exiting!\n");
        return true;
    }
    return false;
}

void get_lib_from_system_dump(char *system_check) {

    int i;
    char system_dump_path_to_blob[128];

    for (i = 0; i < num_blob_directories; i++) {
        sprintf(system_dump_path_to_blob, "%s%s%s", system_dump_root, blob_directories[i],
                system_check);
        if (!access(system_dump_path_to_blob, F_OK)) {
            printf("vendor/manufacturer/device/proprietary%s%s:system%s%s\n", blob_directories[i], system_check,
                    blob_directories[i], system_check);
            dot_so_finder(system_dump_path_to_blob);
            return;
        }
    }

    /* if we've made it this far, it means that the blob was in neither the emulator nor the
     * actual system dump, meaning it is an obsolete reference to a no-longer used blob that
     * was never removed, or more likely, a wildcard in the form of libmmcamera_%s.so, so
     * process the wildcard accordingly, or print out that it's an obsolete reference, or
     * possibly a program fuck-up.
     */
    if (strchr(system_check, '%'))
        process_wildcard(system_check);
    else
        printf("warning: blob file %s missing or broken\n", system_check);
}

void strip_newline(char *line) {

    char *found;

    found = strchr(line, '\n');
    if (found)
        *found = '\0';
}
