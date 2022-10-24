# library
LIBNAME = libzfile.so

# environment
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	CPP = clang++
    CFLAGS = -O
	LDFLAGS = -shared
endif
ifeq ($(UNAME_S),OS/390)
	CPP = xlclang++
    CFLAGS = -+ -O -Wc,'ascii,xplink,arch(8),target(zOSV2R3),list,source,lp64,exportall,gonum,asm,float(ieee),lang(extended0x),offset)' -qmakedep -Iinclude -D__IBMCPP_TR1__ -D_XOPEN_SOURCE=600 -D_XOPEN_SOURCE_EXTENDED
    LDFLAGS = -Wl,'lp64,dll'
endif
INCLUDE = -I../../json/single_include


# objects
OBJS = libzfile.o

# default
default: all

# everything
all: $(LIBNAME)

# link net driver
$(LIBNAME): $(OBJS)
	mkdir -p ../lib/
	$(CPP) $(LDFLAGS) -o $(LIBNAME) $(OBJS)

# clean
RM = rm
RMFLAGS = -f
clean:
	$(RM) $(RMFLAGS) *.o *.lst *.so *.x

# C++ compile
%.o: %.cpp
	$(CPP) $(INCLUDE) $(CFLAGS) -c $*.cpp -o $*.o>$*.lst
