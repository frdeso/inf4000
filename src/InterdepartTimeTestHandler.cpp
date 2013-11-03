#include <arpa/inet.h>
#include <iostream>
#include <sstream> // for ostringstream
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <stdio.h> //scanf
#include <climits>//UINT_MAX

#include <pcap.h>

#include "InterdepartTimeTestHandler.h"
#include "utils.h"
#include "../libs/jsoncpp/json/json.h"

#define MAC_ADDR_LEN_STR (6*2+5+1)
#define IP_ADDR_LEN_STR (4*3+3+1)

using namespace std;

InterdepartTimeTestHandler::InterdepartTimeTestHandler(fs::fstream *modelFile, fs::path path):FeatureTestHandler(modelFile, path){
	interdepTiming_ = new map<uint32_t, map<uint64_t, uint32_t> >();
	interdepTimingCumul_ =  new map<uint32_t, vector<uint64_t> >();

}

void interdepartureDistributionCallback( unsigned char * arg, const struct pcap_pkthdr* pkthdr, const unsigned char * packet )
{
	struct ip * ipHdr = (struct ip*) (packet + sizeof(struct ether_header));

	map<uint32_t, list<uint64_t> > * packetTiming = (map<uint32_t, list<uint64_t> > *) arg;

	(*packetTiming)[ipHdr->ip_src.s_addr].push_back(timevalToUINT64(pkthdr->ts));
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

void InterdepartTimeTestHandler::JsonToData(Json::Value * root){


	//Take the feature section of the model
	Json::Value featureName = (*root)[MODEL_ROOT_][getFeatureName()]; //FIXME: understand what can i do with the "null" situation
	std::vector<string> addressList = featureName.getMemberNames();
	//cout <<"size: "<< addressList.size()<<endl;
	//Add data
	  for (std::vector<string>::iterator it = addressList.begin() ; it != addressList.end(); ++it){
	  	//cout<<*it<<endl;
		for(uint32_t i = 0; i < featureName[it->c_str()].size(); ++i){
			uint32_t address = atoi(it->c_str());
			
			//cout<<featureName[*it][i][1]<<endl;
			uint64_t timing = featureName[*it][i][0].asLargestUInt();
			//cout<<"address: "<<address<<", timing: "<<timing<<endl; 
			(*(interdepTiming_))[address][timing] += featureName[*it][i][1].asUInt();

		}
	}
}


Json::Value* InterdepartTimeTestHandler::DataToJson() const
{
	Json::Value *vect = new Json::Value();

	for ( map<uint32_t ,map<uint64_t, uint32_t> >::iterator it=interdepTiming_->begin(); it!=interdepTiming_->end(); ++it){
		Json::Value addressVec(Json::arrayValue);
		for (map<uint64_t, uint32_t>::iterator itList=it->second.begin(); itList!=it->second.end(); ++itList){
			Json::Value timingVec(Json::arrayValue);
			timingVec.append((Json::LargestUInt)itList->first) ;
			timingVec.append( itList->second);
			addressVec.append(timingVec);
		}
		ostringstream convert;
		convert << it->first;
		(*vect)[convert.str()] = addressVec;
	}
	cout<<vect;
	return vect;
}

int InterdepartTimeTestHandler::getTestResult(){
	//TODO: implemente me
	return 0;
}
void InterdepartTimeTestHandler::runTest(){


}

void InterdepartTimeTestHandler::printDistribution() const
{
	for ( map<uint32_t, map<uint64_t, uint32_t> >::iterator it=interdepTiming_->begin(); it!=interdepTiming_->end(); ++it){
		cout<<"\n\n"<<"adresse :"<< it->first <<" interdep: "<<endl;
		for (  map<uint64_t, uint32_t>::iterator itList=it->second.begin(); itList!=it->second.end(); ++itList){
			cout<<"\t"<<itList->first << " : "<<itList->second<<endl;
		}
	}
}

	
void InterdepartTimeTestHandler::ComputeInterdeparture(map<uint32_t, list<uint64_t> > *packetTiming){
	for ( map<uint32_t, list<uint64_t> >::iterator it=packetTiming->begin(); it!=packetTiming->end(); ++it){
		if(it->second.size() > 1){
			list<uint64_t>::iterator itList = it->second.begin();
			++itList;
			
			for (  ; itList!=it->second.end(); ++itList){ 		//start at the second element
				uint64_t first = *(--itList); // go back one element
				uint64_t second = *(++itList); // return to the current one 
				
				//cout<<"address: "<< it->first<<" , timing: "<< second -first<< endl;
				(*interdepTiming_)[it->first][second - first]++; 
			}
			//(*interdepTiming_)[it->first].sort();
		}
	}

}
string InterdepartTimeTestHandler::getFeatureName() const{
	return FEATURE_NAME;
}
uint32_t InterdepartTimeTestHandler::getModelSampleSize(){
	static uint32_t size = UINT_MAX;
	if(size == UINT_MAX){
		size = 0;
		for ( map<uint32_t, map<uint64_t, uint32_t> >::iterator it=interdepTiming_->begin(); it!=interdepTiming_->end(); ++it){
			for (  map<uint64_t, uint32_t>::iterator itList=it->second.begin(); itList!=it->second.end(); ++itList){
				size += itList->second;
			}
		}
	}
	return size;
}
uint32_t InterdepartTimeTestHandler::getTestSampleSize(){return 0;}

const std::string InterdepartTimeTestHandler::FEATURE_NAME = "INTERDEPARTURE_TIME";
