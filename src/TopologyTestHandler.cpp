//Standard library
#include <arpa/inet.h>
#include <iomanip>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>

#include <vector>
//third parties
#include "../libs/jsoncpp/json/json.h"
#include <pcap.h>

//my includes
#include "constants.h"
#include "TopologyTestHandler.h"
#include "utils.h"


using namespace std;

TopologyTestHandler::TopologyTestHandler(fs::fstream *modelFile, fs::path path):FeatureTestHandler(modelFile, path)
{
	modelTopology_ = std::map<uint64_t,uint64_t>();
	testTopology_ = std::map<uint64_t,uint64_t>();
	unknownInteractions_ = std::map<uint64_t,uint64_t>();
	setFeatureName("TOPOLOGY");
}
TopologyTestHandler::~TopologyTestHandler(){

}
void TopologyTestHandler::initCapture()
{}

void TopologyCallback( unsigned char * arg, const struct pcap_pkthdr* pkthdr, const unsigned char * packet )
{
	std::map<uint64_t,uint64_t>* topo = (std::map<uint64_t,uint64_t> *) arg;
	struct ip * ipHdr = (struct ip*) (packet + sizeof(struct ether_header));
	
	(*topo)[ipHdr->ip_src.s_addr] = ipHdr->ip_dst.s_addr;

 }

void TopologyTestHandler::ComputeDistribution(int type, PacketCapture *packetCapture )
{
	map<uint64_t,uint64_t> * arg;
	if(type == LEARNING_DATA) arg = &modelTopology_;
	if(type == ANALYSIS_DATA) arg = &testTopology_;

	for( vector<pcap_t*>::iterator it = packetCapture->getRawPackets()->begin(); it != packetCapture->getRawPackets()->end(); ++it){
		pcap_loop(*it, -1, TopologyCallback, (unsigned char *)arg);
	}
}

void TopologyTestHandler::JsonToData(Json::Value *root)
{
	Json::Value featureName = (*root)[MODEL_ROOT_][getFeatureName()];
	
	for(unsigned int i = 0; i < featureName.size();i++){
		modelTopology_[featureName[i][0].asLargestUInt()] = (uint64_t) featureName[i][1].asLargestUInt();
	}
}

Json::Value *TopologyTestHandler::DataToJson() const
{
	Json::Value *vect = new Json::Value();
	for(map<uint64_t,uint64_t>::const_iterator it = modelTopology_.begin(); it != modelTopology_.end(); it++){
		Json::Value commVec(Json::arrayValue);
		commVec.append((Json::LargestUInt)it->first);
		commVec.append((Json::LargestUInt)it->second);
		vect->append(commVec);
	}
	return vect;
}

void TopologyTestHandler::runTest(){
	for(map<uint64_t,uint64_t>::const_iterator it = testTopology_.begin(); it != testTopology_.end(); it++){

		if(modelTopology_[it->first] != it->second){
			//cout<< "Unknown interaction";
			unknownInteractions_[it->first] = it->second;
		}else{
		//	cout<< "Known interaction";
		}
		//cout<<setw(15)<<inet_ntoa(first);
		//	cout<<" ----> "<<inet_ntoa(second)<<endl;
	}

}
int TopologyTestHandler::getTestResult(){ 
	for(map<uint64_t,uint64_t>::const_iterator it = unknownInteractions_.begin(); it != unknownInteractions_.end(); it++){
		struct in_addr first, second;
		first.s_addr = it->first;
		second.s_addr = it->second;
		cout<< printInRed("Unknown interaction : ");
		cout<<setw(14)<<inet_ntoa(first);
		cout<<" ----> "<<setw(14)<<inet_ntoa(second)<<endl;
	}

	return 0;
}
void TopologyTestHandler::printDistribution() const
{
	for(map<uint64_t,uint64_t>::const_iterator it = modelTopology_.begin(); it != modelTopology_.end(); it++){
		struct in_addr first, second;
		first.s_addr = it->first;
		second.s_addr = it->second;
		cout<<setw(15)<<inet_ntoa(first);
		cout<<" ----> "<<inet_ntoa(second)<< endl;
	}
}
