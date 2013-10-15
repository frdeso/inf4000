#include "PacketLengthTestHandler.h"
#include "PacketCapture.h"
#include <iostream>
#include <map>

PacketLengthTestHandler::PacketLengthTestHandler():FeatureTestHandler()
{
	packetDistribution_ = new std::map<uint32_t,uint32_t>();
}


void packetDistributionCallback( unsigned char * arg, const struct pcap_pkthdr* pkthdr, const unsigned char * packet )
{
	PacketLengthTestHandler* p = (PacketLengthTestHandler* ) arg;

	(*p->getPacketDistribution())[pkthdr->len]++;
}

void PacketLengthTestHandler::initCapture(){
	ComputePacketDistribution();

}

void PacketLengthTestHandler::saveDataToModel(){
	
}

void PacketLengthTestHandler::ComputePacketDistribution()
{
	for(std::vector<pcap_t*>::iterator it = getPacketCapture()->getRawPackets()->begin(); it != getPacketCapture()->getRawPackets()->end(); ++it){
		pcap_loop(*it, -1, packetDistributionCallback,(unsigned char *) this);
	}
}

std::map<uint32_t ,uint32_t>* PacketLengthTestHandler::getPacketDistribution() const
{
	return packetDistribution_;
}

void PacketLengthTestHandler::printDistribution() const
{
	for (std::map<uint32_t ,uint32_t>::iterator it=packetDistribution_->begin(); it!=packetDistribution_->end(); ++it)
		std::cout << it->first << "\t=> " << it->second << std::endl;
}

int PacketLengthTestHandler::getTestResult(){
	/* TODO */
	return 0;
}

