OBJs := $(patsubst %.cpp, %.o, $(wildcard *.cpp)) model_board/model_board.o
DEPs := $(OBJs:%.o=%.d)
BIN  := ahrs-visualizer

# Use a modern language
CPPFLAGS += --std=c++0x

# Turn on "all" warnings
CPPFLAGS += -Wall

# Fix http://gcc.gnu.org/bugzilla/show_bug.cgi?id=42748
CPPFLAGS += -Wno-psabi

# Generate .d files with dependency info
CPPFLAGS += -MD -MP

# Debuggable and optimized.
CPPFLAGS += -g -O2 

# Weird stuff needed to do OpenGL ES 2 on a Raspberry Pi
CPPFLAGS += -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/interface/vmcs_host/linux
LDFLAGS += -L/opt/vc/lib -lGLESv2

# Tell the cpp file where assets are stored.
CPPFLAGS += -DASSET_DIR="\"$(assetdir)\""

# libpng for reading in textures
LDFLAGS += $(shell libpng-config --libs)

# boost_program_options library for reading command-line arguments.
LDFLAGS += -lboost_program_options

.PHONY: all
all: $(BIN)

$(BIN): $(OBJs)

.PHONY: clean
clean:
	@rm -fv $(BIN) $(OBJs) $(DEPs) *.o

-include $(DEPs)

prefix = $(DESTDIR)/usr
bindir = $(prefix)/bin
sharedir = $(prefix)/share
assetdir = $(sharedir)/$(BIN)
mandir = $(sharedir)/man
man1dir = $(mandir)/man1

.PHONY: install_assets
install_assets:
	install -d $(assetdir)
	install -m 0644 $(wildcard assets/*.png) $(assetdir)

.PHONY: install
install: $(BIN) install_assets
	install $(BIN) $(bindir)
	install -m 0644 $(BIN).1 $(man1dir)

# Options that came from the Raspberry Pi example 3D code but are apparently
# not necessary because I was able to remove them:
#CFLAGS += -DSTANDALONE -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS
#CFLAGS += -DTARGET_POSIX -D_LINUX -fPIC -DPIC -D_REENTRANT -D_LARGEFILE64_SOURCE
#CFLAGS += -D_FILE_OFFSET_BITS=64 -U_FORTIFY_SOURCE
#CFLAGS += -DHAVE_LIBOPENMAX=2 -DOMX -DOMX_SKIP64BIT
#CFLAGS += -ftree-vectorize -pipe -DUSE_EXTERNAL_OMX
#CFLAGS += -DHAVE_LIBBCM_HOST -DUSE_EXTERNAL_LIBBCM_HOST -DUSE_VCHIQ_ARM

# Libraries that we used to link to because the Raspberry Pi OpenGL example used them:
# Most of these are still depencies, as you can see if you run ldd.
#-lopenmaxil -lbcm_host -lvcos, -lvchiq_arm -lEGL /opt/vc/src/hello_pi/libs/ilclient/libilclient.a

# Include directories we used to have:
#CFLAGS += -I/opt/vc/src/hello_pi/libs/ilclient -I/opt/vc/src/hello_pi/libs/ilclient/libs/vgfont
