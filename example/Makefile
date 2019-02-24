src = $(wildcard *.c)
src += $(wildcard ../*.c)
obj = $(src:.c=.o)
inc = -I$(CURDIR)/../
cflags = -Og -g -ggdb -gstabs

default: poly

poly: $(obj)
	$(CC) $(cflags) -o $@ $^  $(inc)
	
.PHONY: clean
clean:
	rm -f $(obj) myprog
