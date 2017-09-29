# Copyright (c) 2003 ACM/SIGDA
#
# Written by Florian Krohm (fkrohm@us.ibm.com)
# Adapted by Geert Janssen (geert@us.ibm.com)
#
# This Makefile will compile all C (*.c) and C++ (*.C) sources in 
# this directory and link them into an executable specified by variable TARGET.
# In order not to have to model the exact dependencies on header files this
# Makefile assumes that all C/C++ files depend on all header files.
#
# make debug   - compile and link to produce debuggable executable
# make opt     - compile and link to produce optimized executable
# make clean   - remove all generated files
# make test    - run all testcases
# make submit  - copy relevant files to solution directory
#
# You may change the value of SUBMIT_FILES to your needs
# You must not modify TARGET or SUBMIT_DIR.
#

TARGET		= buffer

SUBMIT_DIR      = ../solution
SUBMIT_FILES    = $(wildcard *.[Cchyl]) Makefile

#
# Tools used
#
CC		= gcc
CXX		= g++
FLEX		= flex
BISON		= bison
DIFF		= diff

#
# Assemble sources, objects, and headers
#
C_SRCS	       := $(wildcard *.c)
C_OBJS	       := $(C_SRCS:.c=.o)

CXX_SRCS       := $(wildcard *.C)
CXX_OBJS       := $(CXX_SRCS:.C=.o)

HDRS	        = $(wildcard *.h)

INCLUDES	=
LIBS		= -lm

#
# Choose suitable commandline flags 
#
ifeq "$(MAKECMDGOALS)" "opt"
CFLAGS   = -O2
CXXFLAGS = -O2
else
CFLAGS   = -g -W -Wall -pedantic -Wno-unused-parameter
CXXFLAGS = -g -W -Wall -pedantic
endif

.PHONY:	clean test debug opt

debug opt: $(TARGET)

$(TARGET):  $(C_OBJS) $(CXX_OBJS)
	$(CXX) -o $(TARGET) $(C_OBJS) $(CXX_OBJS) $(LIBS)

%.o:%.c
	$(CC) -c $(CFLAGS) $<

%.o:%.C
	$(CXX) -c $(CXXFLAGS) $<

$(C_OBJS) $(CXX_OBJS): $(HDRS)

test:	$(TARGET) 
	@for file in ../test/test*; \
	do \
          f=`basename $$file`; \
	  ./$(TARGET) $$file | $(DIFF) - ../test/result.$$f > /dev/null; \
	  if [ $$? -eq 0 ]; then \
	    echo "$$f passed"; \
          else \
	    echo "$$f FAILED"; \
          fi; \
	done

clean : 
	rm -f *.o $(TARGET)

submit:
	submit $(SUBMIT_DIR) $(SUBMIT_FILES)
