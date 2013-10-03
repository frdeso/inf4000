#include "PacketLengthTestHandler.h"
#include "PacketCapture.h"
#include <iostream>
PacketLengthTestHandler::PacketLengthTestHandler(FILE * fichier) : FeatureTestHandler(fichier)
{
	ComputeMaxPacketSize();
}


void FindMaxPacketSizeCallback( unsigned char * arg, const struct pcap_pkthdr* pkthdr, const unsigned char * packet )
{
	PacketLengthTestHandler* p = (PacketLengthTestHandler* ) arg;
	if(p->getPacketCapture()->getMaxPacketSize() < pkthdr->len) {
		p->getPacketCapture()->setMaxPacketSize(pkthdr->len);
	}
	std::cout<<p->getPacketCapture()->getMaxPacketSize()<<std::endl;
}

void PacketLengthTestHandler::ReadPacketCapture(pcap_t * pcap)
{
	this->SetPacketCapture(pcap);

}
void PacketLengthTestHandler::RunTest(){}

size_t PacketLengthTestHandler::ComputeMaxPacketSize()
{
	pcap_loop(this->getPacketCapture()->getRawData(), -1, FindMaxPacketSizeCallback,(unsigned char *) this);
}
