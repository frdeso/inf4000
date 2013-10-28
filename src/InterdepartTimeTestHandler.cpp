#include <arpa/inet.h>
#include <iostream>
#include <sstream> // for ostringstream
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <stdio.h> //scanf
#include <pcap.h>

#include "InterdepartTimeTestHandler.h"
#include "utils.h"
#include "../libs/jsoncpp/json/json.h"

#define MAC_ADDR_LEN_STR (6*2+5+1)
#define IP_ADDR_LEN_STR (4*3+3+1)

using namespace std;

void interdepartureDistributionCallback( unsigned char * arg, const struct pcap_pkthdr* pkthdr, const unsigned char * packet )
{
	struct ip * ipHdr = (struct ip*) (packet + sizeof(struct ether_header));

	map<uint32_t, list<uint64_t> > * packetTiming = (map<uint32_t, list<uint64_t> > *) arg;

	(*packetTiming)[ipHdr->ip_src.s_addr].push_back( timevalToUINT64(pkthdr->ts));



/*
	char mac[MAC_ADDR_LEN_STR];
	sprintf(mac,"%02x:%02x:%02x:%02x:%02x:%02x",
		    (unsigned)ether->ether_dhost[0],
		    (unsigned)ether->ether_dhost[1],
		    (unsigned)ether->ether_dhost[2],
		    (unsigned)ether->ether_dhost[3],
		    (unsigned)ether->ether_dhost[4],
		    (unsigned)ether->ether_dhost[5]);

	char ip[IP_ADDR_LEN_STR];

	

	cout<<"taille:"<<pkthdr->len<<", mac addres:"<<mac<<",  "<<inet_ntoa(ipHdr->ip_src)<<"->"<<inet_ntoa(ipHdr->ip_dst)<<endl;
	*/

}

void InterdepartTimeTestHandler::ComputeDistribution(int type)
{
	map<uint32_t, list<uint64_t> > * packetTiming =  new map<uint32_t, list<uint64_t> >();
	for( vector<pcap_t*>::iterator it = getPacketCapture()->getRawPackets()->begin(); it != getPacketCapture()->getRawPackets()->end(); ++it){
		pcap_loop(*it, -1, interdepartureDistributionCallback, (unsigned char *)packetTiming);
	}

	ComputeInterdeparture(packetTiming);
	
}
void InterdepartTimeTestHandler::initCapture(){}

InterdepartTimeTestHandler::InterdepartTimeTestHandler(std::fstream *modelFile):FeatureTestHandler(modelFile){
	interdepTiming_ = new map<uint32_t, list<uint64_t> >();

}
void InterdepartTimeTestHandler::JsonToData(Json::Value * root){


	//Take the feature section of the model
	Json::Value featureName = (*root)[MODEL_ROOT_][getFeatureName()]; //FIXME: understand what can i do with the "null" situation
	std::vector<string> addressList = featureName.getMemberNames();
	//Add data
	  for (std::vector<string>::iterator it = addressList.begin() ; it != addressList.end(); ++it){
	  	//cout<<*it<<endl;
		for(uint32_t i = 0; i < featureName[it->c_str()].size(); ++i){
			uint32_t address = atoi(it->c_str());
			uint64_t timing = featureName[*it][i].asLargestUInt();
			//cout<<"address: "<<address<<", timing: "<<timing<<endl; 
			(*(interdepTiming_))[address].push_back(timing);
		}
	}
}


Json::Value* InterdepartTimeTestHandler::DataToJson() const
{
	Json::Value *vect = new Json::Value();

	for ( map<uint32_t ,list<uint64_t> >::iterator it=interdepTiming_->begin(); it!=interdepTiming_->end(); ++it){
		Json::Value subvec(Json::arrayValue);
		for ( list<uint64_t>::iterator itList=it->second.begin(); itList!=it->second.end(); ++itList){
			subvec.append((Json::LargestUInt)*itList);
		}
		ostringstream convert;
		convert << it->first;
		(*vect)[convert.str()] = subvec;
	}
	return vect;
}

int InterdepartTimeTestHandler::getTestResult(){
	//TODO: implemente me
	return 0;
}
void InterdepartTimeTestHandler::runTest(){}

void InterdepartTimeTestHandler::printDistribution() const
{
	for ( map<uint32_t, list<uint64_t> >::iterator it=interdepTiming_->begin(); it!=interdepTiming_->end(); ++it){
		cout<<"\n\n"<<"adresse :"<< it->first <<" interdep: ";
		for ( list<uint64_t>::iterator itList=it->second.begin(); itList!=it->second.end(); ++itList){
			cout<<"\t"<<*itList<<endl;
		}
	}
}

	
void InterdepartTimeTestHandler::ComputeInterdeparture(std::map<uint32_t, std::list<uint64_t> > *packetTiming){
	for ( map<uint32_t, list<uint64_t> >::iterator it=packetTiming->begin(); it!=packetTiming->end(); ++it){
		if(it->second.size() > 1){
			list<uint64_t>::iterator itList = it->second.begin();
			++itList;
			
			for (  ; itList!=it->second.end(); ++itList){ 		//start at the second element
				uint64_t first = *(--itList); // go back one element
				uint64_t second = *(++itList); // return to the current one 
				(*interdepTiming_)[it->first].push_back(second - first); //sustract

			}
			
		}
	}
}
string InterdepartTimeTestHandler::getFeatureName() const{
	return FEATURE_NAME;
}
const std::string InterdepartTimeTestHandler::FEATURE_NAME = "INTERDEPARTURE_TIME";
