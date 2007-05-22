CXX = g++
CXXFLAGS = -O3 -c
SOURCE = ../Source
BINARY = .

all: myginv

myginv: myginv.o involutive32_64.o janettree32_64.o poly32_64.o monom32_64.o variables.o timer.o
	$(CXX) \
            $(BINARY)/myginv.o \
            $(BINARY)/involutive32_64.o \
            $(BINARY)/janettree32_64.o \
            $(BINARY)/poly32_64.o \
            $(BINARY)/monom32_64.o \
            $(BINARY)/variables.o \
            $(BINARY)/timer.o \
            -o $(BINARY)/myginv
	@rm $(BINARY)/*.o

myginv.o: $(SOURCE)/init32_64.h
	$(CXX) $(CXXFLAGS) $(SOURCE)/myginv.cpp -o $(BINARY)/myginv.o

involutive32_64.o: $(SOURCE)/involutive32_64.h $(SOURCE)/janettree32_64.h $(SOURCE)/timer.h
	$(CXX) $(CXXFLAGS) $(SOURCE)/involutive32_64.cpp -o $(BINARY)/involutive32_64.o

janettree32_64.o: $(SOURCE)/janettree32_64.h $(SOURCE)/poly32_64.h
	$(CXX) $(CXXFLAGS) $(SOURCE)/janettree32_64.cpp -o $(BINARY)/janettree32_64.o

poly32_64.o: $(SOURCE)/poly32_64.h $(SOURCE)/monom32_64.h
	$(CXX) $(CXXFLAGS) $(SOURCE)/poly32_64.cpp -o $(BINARY)/poly32_64.o

monom32_64.o: $(SOURCE)/monom32_64.h $(SOURCE)/variables.h $(SOURCE)/util.h
	$(CXX) $(CXXFLAGS) $(SOURCE)/monom32_64.cpp -o $(BINARY)/monom32_64.o

variables.o: $(SOURCE)/variables.h $(SOURCE)/util.h
	$(CXX) $(CXXFLAGS) $(SOURCE)/variables.cpp -o $(BINARY)/variables.o

timer.o: $(SOURCE)/timer.h
	$(CXX) $(CXXFLAGS) $(SOURCE)/timer.cpp  -o $(BINARY)/timer.o
