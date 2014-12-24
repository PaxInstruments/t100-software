#------------------------------------------------------------------------------
# Cross-platform Makefile for t100-software
# <ihsan@kehribar.me> - 2014 
#------------------------------------------------------------------------------
TARGET = t100_basicReadings

all: $(TARGET)

CC  = gcc
CXX = g++

#------------------------------------------------------------------------------
# Linux specific configuration
#------------------------------------------------------------------------------
ifeq ($(shell uname), Linux)
	$(error Your platform $(shell uname) is not supported at this time.)
#------------------------------------------------------------------------------
# OSX specific configuration
#------------------------------------------------------------------------------
else ifeq ($(shell uname), Darwin)
	COBJS = ../hidapi/mac/hid.o
	LIBS  = -framework IOKit -framework CoreFoundation
#------------------------------------------------------------------------------
# Windows specific configuration
#------------------------------------------------------------------------------
else
	$(error Your platform $(shell uname) is not supported at this time.)
endif
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Add .cpp files to this line with .o extension
#------------------------------------------------------------------------------
CPPOBJS += $(TARGET).o ../t100_lib/t100_lib.o 
#------------------------------------------------------------------------------

OBJS     = $(COBJS) $(CPPOBJS)
CFLAGS  += -I ../hidapi/hidapi -I ../t100_lib -Wall -g -c

$(TARGET): $(OBJS)	
	g++ -Wall -g $^ $(LIBS) -o $(TARGET)

$(COBJS): %.o: %.c
	$(CC) $(CFLAGS) $< -o $@

$(CPPOBJS): %.o: %.cpp
	$(CXX) $(CFLAGS) $< -o $@

clean:
	rm -f *.o $(TARGET) $(CPPOBJS) $(COBJS)

.PHONY: clean

#------------------------------------------------------------------------------
