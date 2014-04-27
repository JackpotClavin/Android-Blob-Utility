HEADERS = system_directories.h

default: android-blob-utility

android-blob-utility.o: android-blob-utility.c $(HEADERS)
	gcc -c android-blob-utility.c -o android-blob-utility.o

android-blob-utility: android-blob-utility.o
	gcc android-blob-utility.o -o android-blob-utility

clean:
	-rm -f android-blob-utility.o
	-rm -f android-blob-utility
