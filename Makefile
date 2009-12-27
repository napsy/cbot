CC=gcc
CFLAGS=`pkg-config --cflags glib-2.0 gthread-2.0` -g -O0
LDFLAGS=`pkg-config --libs glib-2.0 gthread-2.0` -g -O0
OBJECTS = $(SOURCES:.c=.o)
SOURCES = main.c \
          connection.c \
          network.c \
          client.c \
          irc.c \
          user.c \
          channel.c
BUILD=cbot

all: $(SOURCES) $(BUILD)

$(BUILD): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@
.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

install:
	cp $(BUILD) /usr/bin/

clean:
	rm -rf *.o
	rm -rf dbus/*.o
	rm -rf $(BUILD)
	
