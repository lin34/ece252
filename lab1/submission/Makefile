# Makefile, ECE252  
# Yiqing Huang

CC = gcc       # compiler
CFLAGS = -Wall -g -std=c99 # compilation flags
LD = gcc       # linker
LDFLAGS = -g   # debugging symbols in build
LDLIBS = -lz   # link with libz

# For students 
LIB_UTIL = zutil.o crc.o
SRCS   = png_info.c cat_png.c find_png.c crc.c zutil.c
#OBJS   = main.o $(LIB_UTIL) 

TARGETS= png_info cat_png find_png

all: ${TARGETS}

png_info: png_info.o $(LIB_UTIL) 
	$(LD) -o $@ $^ $(LDLIBS) $(LDFLAGS)

cat_png: cat_png.o $(LIB_UTIL) 
	$(LD) -o $@ $^ $(LDLIBS) $(LDFLAGS)

find_png: find_png.o $(LIB_UTIL) 
	$(LD) -o $@ $^ $(LDLIBS) $(LDFLAGS)

%.o: %.c 
	$(CC) $(CFLAGS) -c $< 

%.d: %.c
	gcc -MM -MF $@ $<

-include $(SRCS:.c=.d)

.PHONY: clean
clean:
	rm -f *.d *.o $(TARGETS) 
