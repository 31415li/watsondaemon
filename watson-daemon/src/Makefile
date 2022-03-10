LDFLAGS:=-lpopt -luci -liotp-as-device -lpaho-mqtt3as -lblobmsg_json -lubox -lubus
SRCS:=$(wildcard ./*.c)
OBJS:=$(SRCS:.c=.o)
BIN:=watson-daemon

.PHONY: all

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

main.o: opts.h config.h device.h system.h
opts.o: opts.h
config.o: config.h opts.h
device.o: device.h
system.o: system.h