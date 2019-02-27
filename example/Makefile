src = $(wildcard *.c)
src = $(wildcard *.cpp)
src += $(wildcard ../*.c)
src += $(wildcard ../*.cpp)
obj = $(src:.c=.o)
inc = -I$(CURDIR)/../
cflags = -Og -g -ggdb -gstabs

default: poly

poly: $(obj)
	g++ $(cflags) -o $@ $^  $(inc)

.PHONY: clean
clean:
	rm -f $(obj) myprog
