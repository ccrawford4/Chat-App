OBJS = main.o
PROG = app
DEBUG_FLAG = -g

%.o: %.c
	gcc -c $(DEBUG_FLAG) -o $@ $<

$(PROG): $(OBJS)
	gcc $(DEBUG_FLAG) -o $@ $^

clean:
	rm -rf $(OBJS) $(PROG)
