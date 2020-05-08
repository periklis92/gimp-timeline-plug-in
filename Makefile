GIMPTOOL = gimptool-2.0
CC = gcc
MINGW = x86_64-w64-mingw32-gcc

GIMPCFLAGS = `gimptool-2.0 --cflags`
GIMPLIBS = `gimptool-2.0 --libs`

CFLAGS = $(GIMPCFLAGS) -I./
LIBS = $(GIMPLIBS) -lm

ifeq ($(OS),Windows_NT)
$(Windows System Detected)
install_win:
else
UNAME_S:=$(shell uname -s)
ifeq ($(UNAME_S),Linux)
$(Linux System Detected)
install_linux:
endif
endif

build_win:
	$(MINGW) -mwindows $(CFLAGS) -o ./bin/timeline-win.exe timeline.c preview.c frame.c $(LIBS)

build_linux:
	$(CC) $(CFLAGS) -o ./bin/timeline timeline.c preview.c frame.c $(LIBS)

install_linux: build_linux
	$(GIMPTOOL) --install-bin ./bin/timeline

install_win: build_win
	$(GIMPTOOL) --install-bin ./bin/timeline-win.exe