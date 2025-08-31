EXECUTABLE = yasvt
TARGET = /usr/bin
CC = gcc
CFLAGS = -g -Wall -pedantic
LFLAGS = 

SOURCES = $(wildcard src/*.c)
OBJECTS = $(patsubst %.c,%.o,$(SOURCES))
DEPENDENCIES = $(patsubst %.c,%.d,$(SOURCES))

all: $(EXECUTABLE)

$(EXECUTABLE): $(DEPENDENCIES) $(OBJECTS)
	$(CC) $(LFLAGS) -o $(EXECUTABLE) $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.d: %.c
	$(CC) -M $< > $@
	$(CC) -M $< | sed s/\\.o/.d/ >> $@

clean:
	-rm $(OBJECTS) $(DEPENDENCIES) *~

install:
	mv $(EXECUTABLE) $(TARGET)

-include $(DEPENDENCIES)
