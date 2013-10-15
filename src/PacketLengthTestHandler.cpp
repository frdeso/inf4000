#include <iostream>
#include <map>

#include "../libs/jsoncpp/json/json.h"

#include "PacketLengthTestHandler.h"
#include "PacketCapture.h"

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
	Json::Value model;
	Json::Value vec(Json::arrayValue);

	for (std::map<uint32_t ,uint32_t>::iterator it=packetDistribution_->begin(); it!=packetDistribution_->end(); ++it){
		Json::Value subVec(Json::arrayValue);
		subVec.append(it->first);
		subVec.append(it->second);

		vec.append(subVec);

	}
	model["packet_lenght"] = vec;
	std::cout << model << std::endl;
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

