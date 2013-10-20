OBJ= PacketLengthTestHandler.o FeatureTestHandler.o PacketCapture.o jsoncpp.o InterdepartTimeTestHandler.o

CFLAGS= -g -Wall -O0

all: pcap_analyzer.c $(OBJ)
	g++ $(CFLAGS)  pcap_analyzer.c $(OBJ) -lpcap -o pcap_analyzer
PacketLengthTestHandler.o: PacketLengthTestHandler.h PacketLengthTestHandler.cpp
	g++ $(CFLAGS) -c PacketLengthTestHandler.cpp  -o PacketLengthTestHandler.o
InterdepartTimeTestHandler.o: InterdepartTimeTestHandler.h InterdepartTimeTestHandler.cpp
	g++ $(CFLAGS) -c InterdepartTimeTestHandler.cpp  -o InterdepartTimeTestHandler.o
FeatureTestHandler.o: FeatureTestHandler.h FeatureTestHandler.cpp
	g++ $(CFLAGS) -c FeatureTestHandler.cpp -o FeatureTestHandler.o
PacketCapture.o: PacketCapture.h PacketCapture.cpp
	g++ $(CFLAGS) -c PacketCapture.cpp -o PacketCapture.o
jsoncpp.o:
	g++ $(CFLAGS) -c ../libs/jsoncpp/jsoncpp.cpp -I ../libs/jsoncpp/ -o jsoncpp.o
clean:
	rm -f pcap_analyzer $(OBJ)
