BIN:=watson-daemon

.PHONY: all

all: $(BIN)

$(BIN): main.c
	$(CC) $(CFLAGS) main.c -o $(BIN) $(LDFLAGS)