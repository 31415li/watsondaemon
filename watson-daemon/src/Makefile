LDFLAGS:=-luci -liotp-as-device -lpaho-mqtt3as -lblobmsg_json -lubox -lubus
SRCS:=$(wildcard ./*.c)
OBJS:=$(SRCS:.c=.o)
BIN:=watson-daemon

.PHONY: all

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)