//Standard library
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
 #include <sys/socket.h>


#include <vector>
//third parties
#include <pcap.h>

//my includes
#include "TopologyTestHandler.h"

using namespace std;

TopologyTestHandler::TopologyTestHandler(fs::fstream *modelFile, fs::path path):FeatureTestHandler(modelFile, path)
{
	topology_ = std::map<uint64_t,uint64_t>();

}
void TopologyTestHandler::initCapture()
{}

void TopologyCallback( unsigned char * arg, const struct pcap_pkthdr* pkthdr, const unsigned char * packet )
{
	std::map<uint64_t,uint64_t>* topo = (std::map<uint64_t,uint64_t> *) arg;
	struct ip * ipHdr = (struct ip*) (packet + sizeof(struct ether_header));
	
	(*topo)[ipHdr->ip_src.s_addr] = ipHdr->ip_dst.s_addr;

 }

void TopologyTestHandler::ComputeDistribution(int type)
{

	for( vector<pcap_t*>::iterator it = getPacketCapture()->getRawPackets()->begin(); it != getPacketCapture()->getRawPackets()->end(); ++it){
		pcap_loop(*it, -1, TopologyCallback, (unsigned char *)&topology_);
	}
	printDistribution();
}
void TopologyTestHandler::JsonToData(Json::Value * json)
{}
Json::Value *TopologyTestHandler::DataToJson() const
{
	return (Json::Value *)NULL;
}
std::string TopologyTestHandler::getFeatureName() const{
	return "IMPLEMENT ME";
}
void TopologyTestHandler::runTest(){

}
int TopologyTestHandler::getTestResult(){ 
	return 0;
}
void TopologyTestHandler::printDistribution() const
{
	for(map<uint64_t,uint64_t>::const_iterator it = topology_.begin(); it != topology_.end(); it++){
		struct in_addr first, second;
		first.s_addr = it->first;
		second.s_addr = it->second;
		cout<<inet_ntoa(first)<<" ---->"<<inet_ntoa(second)<< endl;
	}
}

const std::string TopologyTestHandler::FEATURE_NAME = "TOPOLOGY";