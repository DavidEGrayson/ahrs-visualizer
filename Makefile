OBJs := $(patsubst %.cpp, %.o, $(wildcard *.cpp)) model_board/model_board.o
DEPs := $(OBJs:%.o=%.d)
BIN  := ahrs-visualizer

# Use a modern language
CPPFLAGS += --std=c++11

# Turn on "all" warnings
CPPFLAGS += -Wall

# Generate .d files with dependency info
CPPFLAGS += -MD -MP

# Debuggable and optimized.
CPPFLAGS += -g -O2

# Weird stuff needed to do OpenGL ES 2 on a Raspberry Pi
CPPFLAGS += -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/interface/vmcs_host/linux
LDFLAGS += -L/opt/vc/lib
LDLIBS += -lGLESv2 -lbcm_host -lEGL -lm -lstdc++

# Tell the cpp file where assets are stored.
CPPFLAGS += -DASSET_DIR="\"$(assetdir)\""

# libpng for reading in textures
LDLIBS += $(shell libpng-config --libs)

# boost_program_options library for reading command-line arguments.
LDLIBS += -lboost_program_options

.PHONY: all
all: $(BIN)

$(BIN): $(OBJs)

.PHONY: clean
clean:
	@rm -fv $(BIN) $(OBJs) $(DEPs) *.o

-include $(DEPs)

prefix = $(DESTDIR)/usr/local
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
