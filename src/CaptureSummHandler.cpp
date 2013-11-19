#include "CaptureSummHandler.h"

using namespace std;

CaptureSummHandler::CaptureSummHandler(fs::fstream *modelFile, fs::path path, int typeOfTest):FeatureTestHandler(modelFile, path, typeOfTest){
	nbOfPackets_ = 0;
	setFeatureName("CAPTURE_SUMMARY");
}
CaptureSummHandler::~CaptureSummHandler(){}
void CaptureSummHandler::initCapture(){}
void CaptureSummHandler::computePacket(const struct pcap_pkthdr* pkthdr, const unsigned char * packet ){
	nbOfPackets_++;
}
void CaptureSummHandler::JsonToData(Json::Value * json){}
Json::Value *CaptureSummHandler::DataToJson() const {
	return (Json::Value *)NULL;
}
void CaptureSummHandler::runTest(){}
int CaptureSummHandler::getTestResult(){
	cout<<"Number of packet captured: "<<nbOfPackets_<<endl;
	return 0;
}
void CaptureSummHandler::printDistribution() const{}