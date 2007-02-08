CXX = g++
CXXFLAGS = -O3 -c
SOURCE = ../Source
BINARY = .

all: myginv

myginv: myginv.o igroebner64.o igroebner128.o janettree64.o janettree128.o imypoly64.o imypoly128.o imymonom64.o imymonom128.o ivariables.o itimer.o
	$(CXX) \
            $(BINARY)/myginv.o \
            $(BINARY)/igroebner64.o \
            $(BINARY)/igroebner128.o \
            $(BINARY)/janettree64.o \
            $(BINARY)/janettree128.o \
            $(BINARY)/imypoly64.o \
            $(BINARY)/imypoly128.o \
            $(BINARY)/imymonom64.o \
            $(BINARY)/imymonom128.o \
            $(BINARY)/ivariables.o \
            $(BINARY)/itimer.o \
            -o $(BINARY)/myginv
	@rm $(BINARY)/*.o

myginv.o: $(SOURCE)/init64.h $(SOURCE)/init128.h $(SOURCE)/ivariables.h
	$(CXX) $(CXXFLAGS) $(SOURCE)/myginv.cpp -o $(BINARY)/myginv.o

igroebner64.o: $(SOURCE)/igroebner64.h $(SOURCE)/janettree64.h
	$(CXX) $(CXXFLAGS) $(SOURCE)/igroebner64.cpp -o $(BINARY)/igroebner64.o

igroebner128.o: $(SOURCE)/igroebner128.h $(SOURCE)/janettree128.h
	$(CXX) $(CXXFLAGS) $(SOURCE)/igroebner128.cpp -o $(BINARY)/igroebner128.o

janettree64.o: $(SOURCE)/janettree64.h $(SOURCE)/imypoly64.h
	$(CXX) $(CXXFLAGS) $(SOURCE)/janettree64.cpp -o $(BINARY)/janettree64.o

janettree128.o: $(SOURCE)/janettree128.h $(SOURCE)/imypoly128.h
	$(CXX) $(CXXFLAGS) $(SOURCE)/janettree128.cpp -o $(BINARY)/janettree128.o

imypoly64.o: $(SOURCE)/imypoly64.h $(SOURCE)/imymonom64.h
	$(CXX) $(CXXFLAGS) $(SOURCE)/imypoly64.cpp -o $(BINARY)/imypoly64.o

imypoly128.o: $(SOURCE)/imypoly128.h $(SOURCE)/imymonom128.h
	$(CXX) $(CXXFLAGS) $(SOURCE)/imypoly128.cpp -o $(BINARY)/imypoly128.o

imymonom64.o: $(SOURCE)/imymonom64.h $(SOURCE)/ivariables.h $(SOURCE)/iutil.h
	$(CXX) $(CXXFLAGS) $(SOURCE)/imymonom64.cpp -o $(BINARY)/imymonom64.o

imymonom128.o: $(SOURCE)/imymonom128.h $(SOURCE)/ivariables.h $(SOURCE)/iutil.h
	$(CXX) $(CXXFLAGS) $(SOURCE)/imymonom128.cpp -o $(BINARY)/imymonom128.o

ivariables.o: $(SOURCE)/ivariables.h $(SOURCE)/iutil.h
	$(CXX) $(CXXFLAGS) $(SOURCE)/ivariables.cpp -o $(BINARY)/ivariables.o

itimer.o: $(SOURCE)/itimer.h
	$(CXX) $(CXXFLAGS) $(SOURCE)/itimer.cpp  -o $(BINARY)/itimer.o
