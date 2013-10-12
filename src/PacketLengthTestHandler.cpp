#include "PacketLengthTestHandler.h"
#include "PacketCapture.h"
#include <iostream>
#include <map>

PacketLengthTestHandler::PacketLengthTestHandler(FILE * fichier) : FeatureTestHandler(fichier)
{
	packetDistribution_ = new std::map<unsigned int, unsigned int>();
	ComputeMaxPacketSize();
}


void FindMaxPacketSizeCallback( unsigned char * arg, const struct pcap_pkthdr* pkthdr, const unsigned char * packet )
{
	PacketLengthTestHandler* p = (PacketLengthTestHandler* ) arg;
	size_t max = 0;

	(*p->getPacketDistribution())[pkthdr->len]++;
//	std::cout<<pkthdr->len<< ": "<<(*p->getPacketDistribution())[pkthdr->len]<<std::endl;
	if(p->getPacketCapture()->getMaxPacketSize() < pkthdr->len) {
		max = pkthdr->len;
	}
	p->getPacketCapture()->setMaxPacketSize(max);

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

std::map<unsigned int , unsigned int>* PacketLengthTestHandler::getPacketDistribution() const
{
	return packetDistribution_;
}
void PacketLengthTestHandler::printDistribution() const
{
}

