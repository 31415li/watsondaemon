LDFLAGS:=-luci -liotp-as-device -lpaho-mqtt3as -lblobmsg_json -lubox -lubus
BIN:=watson-daemon

.PHONY: all

all: $(BIN)

$(BIN): main.c
	$(CC) $(CFLAGS) main.c -o $(BIN) $(LDFLAGS)