all: PacketLengthTestHandler.o FeatureTestHandler.o PacketCapture.o pcap_analyzer.c
	g++ -g -lpcap pcap_analyzer.c PacketLengthTestHandler.o PacketCapture.o -o pcap_analyzer
PacketLengthTestHandler.o : PacketLengthTestHandler.h PacketLengthTestHandler.cpp 
	g++ -g -c PacketLengthTestHandler.cpp  -o PacketLengthTestHandler.o
FeatureTestHandler.o: FeatureTestHandler.h
	g++ -g -c FeatureTestHandler.h -o FeatureTestHandler.o
PacketCapture.o: PacketCapture.h PacketCapture.cpp
	g++ -g -c PacketCapture.cpp -o PacketCapture.o
clean:
	rm -f pcap_analyzer PacketLengthTestHandler.o FeatureTestHandler.o PacketCapture.o
