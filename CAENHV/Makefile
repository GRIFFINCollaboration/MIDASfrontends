#####################################################################
#
#  Name:         Makefile
#  Created by:   Stefan Ritt
#
#  Contents:     UNIX Makefile for MIDAS slow control frontend
#
#  $Id: Makefile 2779 2005-10-19 13:14:36Z ritt $
#
#####################################################################

#--------------------------------------------------------------------
# The following lines contain specific switches for different UNIX
# systems. Find the one which matches your OS and outcomment the 
# lines below.

# This is for Linux ----------------
LIBS = -lbsd -lm -lutil -lpthread
OSFLAGS = -DOS_LINUX

# This is for OSF1 -----------------
#LIBS = -lbsd
#OSFLAGS = -DOS_OSF1

# This is for Ultrix ---------------
#LIBS =
#OSFLAGS = -DOS_ULTRIX -Dextname

# This is for FreeBSD --------------
#LIBS = -lbsd -lcompat
#OSFLAGS = -DOS_FREEBSD

#-------------------------------------------------------------------
# The following lines define direcories. Adjust if necessary
#                 

INC_DIR 	= $(MIDASSYS)/include
LIB_DIR 	= $(MIDASSYS)/linux-m64/lib
DRV_DIR		= $(MIDASSYS)/drivers
CAEN_DIR        = $(HOME)/Linux
#-------------------------------------------------------------------
# Drivers needed by the frontend program
#                 
DRIVERS         = hv.o dd_sy2527.o null.o 

####################################################################
# Lines below here should not be edited
####################################################################

#LIB = $(LIB_DIR)/libmidas.a $(CAEN_DIR)/Lib/x64/libcaenhvwrapper.so.3.10 -Wl,-rpath,$(CAEN_DIR)/Lib/x64 $(CAEN_DIR)/Lib/x64/hscaenetlib.so.1.7 -lrt
LIB = $(LIB_DIR)/libmidas.a $(CAEN_DIR)/lib/x64/libcaenhvwrapper.so.5.22 -Wl,-rpath,$(CAEN_DIR)/lib/x64 $(CAEN_DIR)/lib/x64/hscaenetlib.so.1.7 -ldl -lrt -lm

# compiler
CC = cc
CFLAGS = -g -Wall -DUNIX -I$(INC_DIR) -I$(DRV_DIR) -I$(CAEN_DIR)/include
LDFLAGS =

all: fesy2527-0

fesy2527-0:  $(LIB_DIR)/mfe.o fesy2527.o $(DRIVERS)
	$(CC) -o fesy2527-0 fesy2527.o $(LIB_DIR)/mfe.o $(DRIVERS) $(LIB) $(LDFLAGS) $(LIBS)

hv.o: $(DRV_DIR)/class/hv.c
	$(CC) $(CFLAGS) $(OSFLAGS) -c $< -o $@

dd_sy2527.o: $(DRV_DIR)/device/dd_sy2527.c
	$(CC) $(CFLAGS) $(OSFLAGS) -c $< -o $@

nulldev.o: $(DRV_DIR)/device/nulldev.c
	$(CC) $(CFLAGS) $(OSFLAGS) -c $< -o $@

null.o: $(DRV_DIR)/bus/null.c
	$(CC) $(CFLAGS) $(OSFLAGS) -c $< -o $@

.c.o:
	$(CC) $(CFLAGS) $(OSFLAGS) -c $<

clean:
	rm -f *.o *~ \#*

