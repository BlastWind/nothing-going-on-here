CC     = gcc
INC    = -I.
CFLAGS = $(INC) -Wall -Wextra -Werror -pthread
CFILES = $(wildcard *.c)
OBJS   = $(patsubst %.c, %.o,$(CFILES))
HEADERS = $(wildcard *.h)
BIN    = test
LOCKFLAG = -D LOCKS

# %.o:%.c
# 	$(info Compiling $<)
# 	@$(CC) $(CFLAGS) -o $@ -c $< $(LOCKFLAG)

lock:$(OBJS) list.h
	$(CC) $(CFLAGS) -o $(BIN) $^ $(LOCKFLAG)

nolock:$(OBJS) $@_list.h
	$(CC) $(CFLAGS) -o $(BIN) $^

$(BIN):$(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

exec:$(BIN)
	./test

clean:
	@rm -f $(BIN) $(OBJS) *~
