OBJs := $(patsubst %.c, %.o, $(wildcard *.c)) model_board/model_board.o
DEPs := $(OBJs:%.o=%.d)
BIN  := ahrs-visualizer

# Let the C file know where it is in the directory structure
# so it can access files in the same directory.
CFLAGS += -DPATH="\"$(dir $@)\""

# Turn on "all" warnings
CFLAGS += -Wall

# Fix http://gcc.gnu.org/bugzilla/show_bug.cgi?id=42748
CFLAGS += -Wno-psabi

# Generate .d files with dependency info
CFLAGS += -MD -MP

CFLAGS += -I/opt/vc/include/ -I/opt/vc/include/interface/vcos/pthreads

CFLAGS += -g -Wno-deprecated-declarations

LDFLAGS += -L/opt/vc/lib/ -lGLESv2 

LDFLAGS += $(shell libpng-config --libs)

all: $(BIN)

$(BIN): $(OBJs)

clean:
	@rm -fv $(BIN) $(OBJs) $(DEPs)*.o

-include $(DEPs)


# Options that came from the Raspberry Pi example code
# (I'm not sure they are necessary)
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
