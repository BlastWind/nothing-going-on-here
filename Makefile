CC     = gcc
INC    = -I.
CFLAGS = $(INC) -Wall -Wextra -Werror -pthread
CFILES = $(wildcard *.c)
OBJS   = $(patsubst %.c, %.o,$(CFILES))
BIN    = test
LOCKFLAG = -D list_version=1
NOLOCKFLAG = -D list_version=0

%.o:%.c
    $(info Compiling $<)
    @$(CC) $(CFLAGS) -o $@ -c $< $(LOCKFLAG)

lock:$(OBJS)
    $(CC) $(CFLAGS) -o $(BIN) $^ $(LOCKFLAG)

nolock:$(OBJS)
    $(CC) $(CFLAGS) -o $(BIN) $^ $(NOLOCKFLAG)
$(BIN):$(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

exec:$(BIN)
	./test

clean:
	@rm -f $(BIN) $(OBJS) *~
