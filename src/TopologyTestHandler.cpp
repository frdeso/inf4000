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
//#include <pcap.h>

//my includes
#include "constants.h"
#include "PacketCapture.h"
#include "TopologyTestHandler.h"
#include "utils.h"

using namespace std;

TopologyTestHandler::TopologyTestHandler(fs::fstream *modelFile, fs::path path, int typeOfTest):FeatureTestHandler(modelFile, path, typeOfTest)
{
	modelTopology_ = new std::map<uint64_t,uint64_t>();
	testTopology_ = new std::map<uint64_t,uint64_t>();
	unknownInteractions_ = new std::map<uint64_t,uint64_t>();
	setFeatureName("TOPOLOGY");
}
TopologyTestHandler::~TopologyTestHandler(){

}
void TopologyTestHandler::initCapture()
{}

void TopologyTestHandler::computePacket(const struct pcap_pkthdr* pkthdr, const unsigned char * packet ) {
	map<uint64_t,uint64_t> * arg;

	if(getTypeOfData() == LEARNING_DATA) 
		arg = modelTopology_;
	if(getTypeOfData() == ANALYSIS_DATA)
		arg = testTopology_;

	map<uint64_t,uint64_t>* topo = (map<uint64_t,uint64_t> *) arg;
	struct ip * ipHdr = (struct ip*) (packet + sizeof(struct ether_header));
	(*topo)[ipHdr->ip_src.s_addr] = ipHdr->ip_dst.s_addr;
}
void TopologyTestHandler::JsonToData(Json::Value *root)
{
	Json::Value featureName = (*root)[MODEL_ROOT_][getFeatureName()];
	
	for(unsigned int i = 0; i < featureName.size();i++){
		(*modelTopology_)[featureName[i][0].asLargestUInt()] = (uint64_t) featureName[i][1].asLargestUInt();
	}
}

Json::Value *TopologyTestHandler::DataToJson() const
{
	Json::Value *vect = new Json::Value();
	for(map<uint64_t,uint64_t>::const_iterator it = modelTopology_->begin(); it != modelTopology_->end(); it++){
		Json::Value commVec(Json::arrayValue);
		commVec.append((Json::LargestUInt)it->first);
		commVec.append((Json::LargestUInt)it->second);
		vect->append(commVec);
	}
	return vect;
}

void TopologyTestHandler::runTest(double cAlpha){
	for(map<uint64_t,uint64_t>::const_iterator it = testTopology_->begin(); it != testTopology_->end(); it++){

		if((*modelTopology_)[it->first] != it->second){
			//cout<< "Unknown interaction";
			(*unknownInteractions_)[it->first] = it->second;
		}else{
		//	cout<< "Known interaction";
		}
		//cout<<setw(15)<<inet_ntoa(first);
		//	cout<<" ----> "<<inet_ntoa(second)<<endl;
	}

}
int TopologyTestHandler::getTestResult(){ 

	if(unknownInteractions_->size() == 0){
		cout<<GREEN<<"The Test distrition matches with model."<<NO_COLOR<<endl;
	}
	else {
		for(map<uint64_t,uint64_t>::const_iterator it = unknownInteractions_->begin(); it != unknownInteractions_->end(); it++){
			struct in_addr first, second;
			first.s_addr = it->first;
			second.s_addr = it->second;
			cout<< RED<<"Unknown interaction : "<<NO_COLOR;
			cout<<setw(14)<<inet_ntoa(first);
			cout<<" ----> "<<setw(14)<<inet_ntoa(second)<<endl;
		}
	}
	return 0;
}
void TopologyTestHandler::printDistribution() const
{
	for(map<uint64_t,uint64_t>::const_iterator it = modelTopology_->begin(); it != modelTopology_->end(); it++){
		struct in_addr first, second;
		first.s_addr = it->first;
		second.s_addr = it->second;
		cout<<setw(15)<<inet_ntoa(first);
		cout<<" ----> "<<inet_ntoa(second)<< endl;
	}
}
