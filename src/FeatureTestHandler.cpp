#include "FeatureTestHandler.h"
#include <iostream>
	
using namespace std;
FeatureTestHandler::FeatureTestHandler(fstream * modelFile){
	pcap_ = new PacketCapture();
	modelFile_ = modelFile;
}

PacketCapture *FeatureTestHandler::getPacketCapture() const
{
	return pcap_;
}
std::fstream *FeatureTestHandler::getModelFile()const{
	return modelFile_;
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

const string FeatureTestHandler::MODEL_ROOT_ = "model";