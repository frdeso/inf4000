#include <iostream>
#include <map>

#include "../libs/jsoncpp/json/json.h"

#include "PacketLengthTestHandler.h"
#include "PacketCapture.h"

#define FEATURE_NAME "PACKET_LENGHT"


using namespace std;
PacketLengthTestHandler::PacketLengthTestHandler(fstream *modelFile):FeatureTestHandler(modelFile)
{
	packetDistribution_ = new std::map<uint32_t,uint32_t>();
}


void packetDistributionCallback( unsigned char * arg, const struct pcap_pkthdr* pkthdr, const unsigned char * packet )
{
	PacketLengthTestHandler* p = (PacketLengthTestHandler* ) arg;

	(*p->getPacketDistribution())[pkthdr->len]++;
}

void PacketLengthTestHandler::initCapture(){
	loadDataToModel();
	ComputePacketDistribution();
}
void PacketLengthTestHandler::loadDataToModel(){
    Json::Value root;
    Json::Reader reader;
    if(getModelFile()->peek() == std::ifstream::traits_type::eof()) {
    	std::cout  << "Empty model file." << endl;
    	Json::Value a = root["model"];
    	return;
    } 
    bool parsingSuccessful = reader.parse( (*getModelFile()), root, false );

    if(root.size() == 0 || root["model"].isNull()) {
    	std::cout  << "File not empty, but no model found" << endl;
    	return;
    } else if ( !parsingSuccessful ) {
    	std::cout  <<"Parsing error :" <<reader.getFormatedErrorMessages() << endl;
    	return;
    } else{
   
	    //Take the feature section of the model
		Json::Value featureName = root["model"][FEATURE_NAME]; //FIXME: understand what can i do with the "null" situation

		//Add data
		for ( unsigned int i = 0; i < featureName.size(); i++ ) {
			//cout << "Loading of model data : " << featureName[i][0].asUInt() <<":"<< featureName[i][1].asUInt() <<endl;
			uint32_t packetlength = featureName[i][0].asUInt();
			uint32_t currentNumber = featureName[i][1].asUInt();
			(*getPacketDistribution())[packetlength] += currentNumber;
		}
	}

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
	model[FEATURE_NAME] = vec;
	Json::Value root;
	root["model"] = model;
	std:cout << root << std::endl;
	getModelFile()->seekg(0, getModelFile()->beg);
	(*getModelFile()) << root ;
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

