OBJ = PacketLengthTestHandler.o FeatureTestHandler.o PacketCapture.o jsoncpp.o InterdepartTimeTestHandler.o TopologyTestHandler.o TestHandlerContainer.o CaptureSummHandler.o

CFLAGS= -g -Wall -O0 -std=c++11

all: pcap_analyzer.c $(OBJ)
	g++ $(CFLAGS) pcap_analyzer.c $(OBJ) -lboost_system  -lboost_filesystem -lpcap  -o pcap_analyzer

PacketLengthTestHandler.o: PacketLengthTestHandler.h PacketLengthTestHandler.cpp FeatureTestHandler.o
	g++ $(CFLAGS) -c PacketLengthTestHandler.cpp  -o PacketLengthTestHandler.o

InterdepartTimeTestHandler.o: InterdepartTimeTestHandler.h InterdepartTimeTestHandler.cpp utils.h FeatureTestHandler.o
	g++ $(CFLAGS) -c InterdepartTimeTestHandler.cpp  -o InterdepartTimeTestHandler.o

TopologyTestHandler.o: TopologyTestHandler.h TopologyTestHandler.cpp utils.h FeatureTestHandler.o
	g++ $(CFLAGS) -c TopologyTestHandler.cpp -o TopologyTestHandler.o

FeatureTestHandler.o: FeatureTestHandler.h FeatureTestHandler.cpp
	g++ $(CFLAGS) -c FeatureTestHandler.cpp -o FeatureTestHandler.o

PacketCapture.o: PacketCapture.h PacketCapture.cpp
	g++ $(CFLAGS) -c PacketCapture.cpp -o PacketCapture.o

CaptureSummHandler.o: CaptureSummHandler.h CaptureSummHandler.cpp FeatureTestHandler.h
	g++ $(CFLAGS) -c CaptureSummHandler.cpp -o CaptureSummHandler.o

TestHandlerContainer.o: TestHandlerContainer.h TestHandlerContainer.cpp FeatureTestHandler.h
	g++ $(CFLAGS) -c TestHandlerContainer.cpp -o TestHandlerContainer.o

jsoncpp.o:
	g++ $(CFLAGS) -c ../libs/jsoncpp/jsoncpp.cpp -I ../libs/jsoncpp/ -o jsoncpp.o
clean:
	rm -f pcap_analyzer $(OBJ)
