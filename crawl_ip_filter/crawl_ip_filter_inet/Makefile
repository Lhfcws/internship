##########################
# Makefile of IPFilter
# @author: Lhfcws Wu (宸风)
# @time: 2013-07-16
##########################

PROGRAM=Crawl_IP_Filter
TARGET=bin/
SRC=src/
CXX=g++
CPPFLAGS=-g -Wall
CXXF=$(CXX) $(CPPFLAGS)

$(PROGRAM): main.o ipfilter.o
	$(CXXF) -o $(TARGET)$(PROGRAM) $(TARGET)main.o $(TARGET)ipfilter.o

main.o: ipfilter.o
	$(CXXF) -o $(TARGET)main.o -c $(SRC)main.cpp

ipfilter.o: 
	$(CXXF) -o $(TARGET)ipfilter.o -c $(SRC)ipfilter.cpp 
	
clean:
	rm -Rf bin/*

.PHONY: clean
