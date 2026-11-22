# ALSB3 Apex Tuner OpenOrbis Makefile
# Developed by: alsb3 dev
TITLE       := ALSB3 Apex Tuner (alsb3 dev)
VERSION     := 02.00
TITLE_ID    := APEX00001
CONTENT_ID  := IV0000-APEX00001_00-ALSB3APEX0000000

# Linked libraries
LIBS        := -lc -lkernel -lc++ -lSceUserService -lSceVideoOut -lSceAudioOut -lScePad -lSceSysmodule -lSceNet -lSceNetCtl -lSceHttp -lSceSsl -lSceFreeType -lSDL2 -lSDL2_image

TOOLCHAIN   := $(OO_PS4_TOOLCHAIN)
PROJDIR     := src
INTDIR      := build
SQLITE_DIR  := third_party/sqlite
SQLITE_SRC  := $(SQLITE_DIR)/sqlite3.c
SDL_TTF_DIR := third_party/SDL_ttf
SDL_TTF_SRC := $(SDL_TTF_DIR)/SDL_ttf.c

CPPFILES    := $(wildcard $(PROJDIR)/*.cpp)
CPP_OBJS    := $(patsubst $(PROJDIR)/%.cpp, $(INTDIR)/%.o, $(CPPFILES))
OBJS        := $(CPP_OBJS) $(INTDIR)/sqlite3.o $(INTDIR)/SDL_ttf.o

TARGET_FLAGS := --target=x86_64-pc-freebsd12-elf -fPIC -funwind-tables -isysroot $(TOOLCHAIN) -isystem $(TOOLCHAIN)/include -D__ORBIS__ -D_GNU_SOURCE -D_POSIX_C_SOURCE=200809L
CXXFLAGS    := $(TARGET_FLAGS) -c -Iinclude -I$(SQLITE_DIR) -isystem $(TOOLCHAIN)/include/c++/v1 -std=c++17
SQLITEFLAGS := $(TARGET_FLAGS) -c -I$(SQLITE_DIR) -DSQLITE_THREADSAFE=0 -DSQLITE_DEFAULT_MEMSTATUS=0
SDL_TTFFLAGS := $(TARGET_FLAGS) -c -I$(TOOLCHAIN)/include/SDL2 -I$(TOOLCHAIN)/include/freetype
LDFLAGS     := -m elf_x86_64 -pie --script $(TOOLCHAIN)/link.x --eh-frame-hdr -L$(TOOLCHAIN)/lib $(LIBS) $(TOOLCHAIN)/lib/crt1.o

CC          := clang
CCX         := clang++
LD          := ld.lld
CDIR        := linux

PKGTOOL     := env DOTNET_SYSTEM_GLOBALIZATION_INVARIANT=1 $(TOOLCHAIN)/bin/$(CDIR)/PkgTool.Core

all: ALSB3_Apex_Tuner.pkg

ALSB3_Apex_Tuner.pkg: pkg.gp4
	$(PKGTOOL) pkg_build $< .
	@if [ -f "$(CONTENT_ID).pkg" ]; then mv "$(CONTENT_ID).pkg" ALSB3_Apex_Tuner.pkg; fi

pkg.gp4: eboot.bin sce_sys/param.sfo
	$(TOOLCHAIN)/bin/$(CDIR)/create-gp4 -out $@ --content-id=$(CONTENT_ID) --files "$^"

sce_sys/param.sfo: Makefile
	mkdir -p sce_sys
	$(PKGTOOL) sfo_new $@
	$(PKGTOOL) sfo_setentry $@ APP_TYPE --type Integer --maxsize 4 --value 1
	$(PKGTOOL) sfo_setentry $@ APP_VER --type Utf8 --maxsize 8 --value '$(VERSION)'
	$(PKGTOOL) sfo_setentry $@ ATTRIBUTE --type Integer --maxsize 4 --value 0
	$(PKGTOOL) sfo_setentry $@ CATEGORY --type Utf8 --maxsize 4 --value 'gd'
	$(PKGTOOL) sfo_setentry $@ CONTENT_ID --type Utf8 --maxsize 48 --value '$(CONTENT_ID)'
	$(PKGTOOL) sfo_setentry $@ DOWNLOAD_DATA_SIZE --type Integer --maxsize 4 --value 0
	$(PKGTOOL) sfo_setentry $@ SYSTEM_VER --type Integer --maxsize 4 --value 0
	$(PKGTOOL) sfo_setentry $@ TITLE --type Utf8 --maxsize 128 --value '$(TITLE)'
	$(PKGTOOL) sfo_setentry $@ TITLE_ID --type Utf8 --maxsize 12 --value '$(TITLE_ID)'
	$(PKGTOOL) sfo_setentry $@ VERSION --type Utf8 --maxsize 8 --value '$(VERSION)'

eboot.bin: $(OBJS)
	$(LD) $(OBJS) -o $(INTDIR)/ALSB3_Apex.elf $(LDFLAGS)
	$(TOOLCHAIN)/bin/$(CDIR)/create-fself -in=$(INTDIR)/ALSB3_Apex.elf -out=$(INTDIR)/ALSB3_Apex.oelf --eboot "eboot.bin" --paid 0x3800000000000011

$(INTDIR)/%.o: $(PROJDIR)/%.cpp
	mkdir -p $(INTDIR)
	$(CCX) $(CXXFLAGS) -o $@ $<

$(INTDIR)/sqlite3.o: $(SQLITE_SRC)
	mkdir -p $(INTDIR)
	$(CC) $(SQLITEFLAGS) -o $@ $<

$(INTDIR)/SDL_ttf.o: $(SDL_TTF_SRC)
	mkdir -p $(INTDIR)
	$(CC) $(SDL_TTFFLAGS) -o $@ $<

clean:
	rm -rf build ALSB3_Apex_Tuner.pkg $(CONTENT_ID).pkg pkg.gp4 eboot.bin sce_sys/param.sfo
