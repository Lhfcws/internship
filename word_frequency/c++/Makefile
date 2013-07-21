##########################
# Makefile of WFStatistic
# @author: Lhfcws Wu (宸风)
# @time: 2013-07-14
##########################

PROGRAM=WFStatistic
TARGET=bin/
SRC=src/
CXX=g++
CPPFLAGS=-g -Wall
CXXF=$(CXX) $(CPPFLAGS)

$(PROGRAM): main.o wfstatistic.o
	$(CXXF) -o $(TARGET)$(PROGRAM) $(TARGET)main.o $(TARGET)wfstatistic.o

main.o: wfstatistic.o
	$(CXXF) -o $(TARGET)main.o -c $(SRC)main.cpp

wfstatistic.o: 
	$(CXXF) -o $(TARGET)wfstatistic.o -c $(SRC)wfstatistic.cpp 
	
clean:
	rm -Rf bin/*

.PHONY: clean
