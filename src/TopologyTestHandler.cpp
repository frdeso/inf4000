//Standard library
#include <arpa/inet.h>
#include <iomanip>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>

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
	modelTopology_ = new std::map<uint64_t,std::set<uint64_t> >();
	testTopology_ = new std::map<uint64_t,std::set<uint64_t> >();
	unknownInteractions_ = new std::map<uint64_t, std::set<uint64_t> >();
	setFeatureName("TOPOLOGY");
}
TopologyTestHandler::~TopologyTestHandler(){

}
void TopologyTestHandler::initCapture()
{}

void TopologyTestHandler::computePacket(const struct pcap_pkthdr* pkthdr, const unsigned char * packet ) {
	map<uint64_t,std::set<uint64_t> > * arg;

	if(getTypeOfData() == LEARNING_DATA) 
		arg = modelTopology_;
	if(getTypeOfData() == ANALYSIS_DATA)
		arg = testTopology_;

	map<uint64_t,std::set<uint64_t> >* topo = (map<uint64_t,std::set<uint64_t> > *) arg;

	struct ip * ipHdr = (struct ip*) (packet + sizeof(struct ether_header));
	(*topo)[ipHdr->ip_src.s_addr].insert(ipHdr->ip_dst.s_addr);

    return;
}
void TopologyTestHandler::JsonToData(Json::Value *root)
{
	Json::Value featureName = (*root)[MODEL_ROOT_][getFeatureName()];
	
	std::vector<string> addressList = featureName.getMemberNames();
	//Add data
	  for (std::vector<string>::iterator it = addressList.begin() ; it != addressList.end(); ++it){
		for(uint32_t i = 0; i < featureName[it->c_str()].size(); ++i){
			uint64_t addrSrc= atoi(it->c_str());
			uint64_t addrDest = featureName[it->c_str()][i][0].asLargestUInt();

			(*modelTopology_)[addrSrc].insert( addrDest);
		}
	}
}

Json::Value *TopologyTestHandler::DataToJson() const
{
	Json::Value *vect = new Json::Value();
	for(map<uint64_t,std::set<uint64_t> >::const_iterator it = modelTopology_->begin(); it != modelTopology_->end(); it++){
		Json::Value commVec(Json::arrayValue);
		for (set<uint64_t>::const_iterator itList=it->second.begin(); itList!=it->second.end(); ++itList){
			Json::Value destiVect(Json::arrayValue);
			destiVect.append((Json::LargestUInt)*itList) ;
			//destiVect.append(1);
			commVec.append(destiVect);
		}
		ostringstream convert;
		convert << it->first;
		(*vect)[convert.str()] = commVec;
	}
	return vect;
}

void TopologyTestHandler::runTest(double cAlpha){

	for(map<uint64_t,std::set<uint64_t> >::const_iterator itTest = testTopology_->begin(); itTest != testTopology_->end(); itTest++){
		for(std::set<uint64_t>::const_iterator itVecTest = itTest->second.begin(); itVecTest != itTest->second.end(); itVecTest++){ 
			bool unknownIntera = true;
			for(std::set<uint64_t>::const_iterator itVecModel = (*modelTopology_)[itTest->first].begin(); itVecModel !=(*modelTopology_)[itTest->first].end(); itVecModel++) {
				if(*itVecModel == *itVecTest ){
					unknownIntera = false;
					break;
				}
			}
			if(unknownIntera == true){
				(*unknownInteractions_)[itTest->first].insert(*itVecTest);
			}		
		}
	}
}
int TopologyTestHandler::getTestResult(){ 

	if(unknownInteractions_->size() == 0){
		cout<<GREEN<<"The Test distrition matches with model."<<NO_COLOR<<endl;
	}
	else {
		for(map<uint64_t,std::set<uint64_t> >::const_iterator it = unknownInteractions_->begin(); it != unknownInteractions_->end(); it++){
			for(std::set<uint64_t>::const_iterator itVecTest = it->second.begin(); itVecTest != it->second.end(); itVecTest++){ 
				struct in_addr first, second;
				first.s_addr = it->first;
				second.s_addr = *itVecTest;
				cout<< RED<<"Unknown interaction : "<<NO_COLOR;
				cout<<setw(14)<<inet_ntoa(first);
				cout<<" ----> "<<setw(14)<<inet_ntoa(second)<<endl;
			}
		}
	}
	return 0;
}
void TopologyTestHandler::printDistribution() const
{
	for(map<uint64_t,std::set<uint64_t> >::const_iterator it = modelTopology_->begin(); it != modelTopology_->end(); it++){
		for(std::set<uint64_t>::const_iterator itVecTest = it->second.begin(); itVecTest != it->second.end(); itVecTest++){ 
			struct in_addr first, second;
			first.s_addr = it->first;
			second.s_addr = *itVecTest;
			cout<<setw(15)<<inet_ntoa(first);
			cout<<" ----> "<<inet_ntoa(second)<< endl;
		}
	}
}
