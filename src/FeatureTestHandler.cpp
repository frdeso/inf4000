#include "FeatureTestHandler.h"
#include <iostream>
	
using namespace std;
FeatureTestHandler::FeatureTestHandler(){
	pcap_ = new PacketCapture();
}
void FeatureTestHandler::initCapture() {cout<<"Unimplemented method. Should be virtual pure?"<<endl;}
void FeatureTestHandler::saveDataToModel(){cout<<"Unimplemented method. Should be virtual pure?"<<endl;}
PacketCapture *FeatureTestHandler::getPacketCapture() const
{
	return pcap_;
}
void FeatureTestHandler::addPacketCaptureFile(FILE * pcapFile)
{
	char error[PCAP_ERRBUF_SIZE];
	
	pcap_t* p = pcap_fopen_offline(pcapFile, error);

	if(p == NULL) {
		cerr<<error<<endl;
	} else {
		this->getPacketCapture()->addRawPacket(p);
	}
}
