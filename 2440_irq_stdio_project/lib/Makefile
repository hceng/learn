objs := div64.o lib1funcs.o ctype.o muldi3.o printf.o string.o vsprintf.o
$(info echo "lib_makefile_objs="$(objs))
libc.a: $(objs)
	${AR} -r -o $@ $^
	
%.o:%.c
	${CC} $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

%.o:%.S
	${CC} $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

clean:
	rm -f libc.a *.o		
	
