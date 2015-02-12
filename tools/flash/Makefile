CFLAGS+=-g
CFLAGS+=-DDEBUG
CFLAGS+=-std=gnu99
CFLAGS+=-Wall -Wextra
CFLAGS+=-I../libstlink/src

LDFLAGS=-L../libstlink -lstlink

# libusb location
LDFLAGS+=`pkg-config --libs libusb-1.0`
CFLAGS+=`pkg-config --cflags libusb-1.0`

SRCS=main.c
OBJS=$(SRCS:.c=.o)

NAME=st-flash

all: $(NAME)

$(NAME): $(OBJS) ../libstlink/libstlink.a
	@echo "bulding ${NAME}"
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJS) $(LDFLAGS)

%.o: %.c
	@$(CC) $(CFLAGS) -c $^ -o $@

clean:
	rm -f $(OBJS)
	rm -f $(NAME)
	make -C ../libstlink clean


../libstlink/libstlink.a:
	make -C ../libstlink

.PHONY: clean all
