SRCS = $(wildcard *.c)
OBJS = $(SRCS:%.o=%.c)

EXECUTABLE = wofost
CC       = gcc
CFLAGS  = -g -ggdb -O0 -Wall -Wextra -lnetcdf -fno-stack-protector -std=c99 \
#CFLAGS   = -Wall -O3

all: $(EXECUTABLE)

$(EXECUTABLE) : $(OBJS)
	$(CC) -o $(EXECUTABLE) $(OBJS) -lm $(CFLAGS)


%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

TAGS:	$(SRCS)
	etags $(SRCS)
clean::
	\rm -f TAGS $(EXECUTABLE)
