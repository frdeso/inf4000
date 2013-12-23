#include <arpa/inet.h>
#include <iostream>
#include <net/ethernet.h>
#include <signal.h>
#include <unistd.h>

#include <pcap.h>

#include "TestHandlerContainer.h"
#include "utils.h"

using namespace std;

TestHandlerContainer::TestHandlerContainer(fs::fstream *modelFile, fs::path path, int type, double alpha){
	path_ = path;
	modelFile_ = modelFile;
	type_ = type;
	handlers_ =  list<FeatureTestHandler *>();
	packetCapture_ = new PacketCapture();
	setCAlpha(alpha);
}

void TestHandlerContainer::addTestHandler(FeatureTestHandler * handler){
	handlers_.push_back(handler);
}

void TestHandlerContainer::addPacketCapture(pcap_t * captureHandle){

	packetCapture_->addRawPacket(captureHandle);
}

void TestHandlerContainer::loadModel(){
	for(list<FeatureTestHandler *>::iterator it = handlers_.begin(); it != handlers_.end(); it++ ){
		(*it)->loadDataFromModel();
	}
}

void containerCallback(unsigned char * arg, const struct pcap_pkthdr* pkthdr, const unsigned char * packet ){
	std::list<FeatureTestHandler* > *handlers = (std::list<FeatureTestHandler* > *) arg;
	//Sending the packet to all the feature handler
	for(list<FeatureTestHandler *>::iterator it = handlers->begin(); it != handlers->end(); it++ ){
		
		struct  ether_header* ethHdr = (struct ether_header*) packet;
		//We check if this is a IP packet. (Removing ARP and DNS...)
		if (ntohs (ethHdr->ether_type) == ETHERTYPE_IP)
		{
			(*it)->computePacket(pkthdr, packet);
		}
		
	}
}

static pcap_t* current_capture;
void alarm_handler(int sig){
	pcap_breakloop(current_capture);
}

void TestHandlerContainer::computeDistribution(bool isLive, uint32_t duration){

	if(isLive){
		//set sigalrm handler
		signal(SIGALRM, alarm_handler);
		//set alarm
		alarm(duration);
	}

	std::list<FeatureTestHandler* > *arg = &handlers_;
	//call pcap_loop on every pcap files
	for( vector<pcap_t*>::iterator it = packetCapture_->getRawPackets()->begin(); it != packetCapture_->getRawPackets()->end(); ++it){
		current_capture =  *it;
		pcap_loop(*it, -1, containerCallback,(unsigned char *) arg);
	}

	for(list<FeatureTestHandler *>::iterator it = handlers_.begin(); it != handlers_.end(); it++ ){
		(*it)->initCapture();
	}
}

void TestHandlerContainer::saveModel(){
	for(list<FeatureTestHandler *>::iterator it = handlers_.begin(); it != handlers_.end(); it++ ){
		(*it)->saveDataToModel();
	}
}

void TestHandlerContainer::runTests(){
	for(list<FeatureTestHandler *>::iterator it = handlers_.begin(); it != handlers_.end(); it++ ){
		(*it)->runTest(cAlpha_);
	}
}

void TestHandlerContainer::printTestsResults(){
	for(list<FeatureTestHandler *>::iterator it = handlers_.begin(); it != handlers_.end(); it++ ){
		string header = " ###### ";
		cout<<YELLOW<<header<<(*it)->getFeatureName()<<header<<NO_COLOR<<endl;
		(*it)->getTestResult();
	}	
}

void TestHandlerContainer::setCAlpha(double alpha){
	if(alpha == 0.10) cAlpha_ 		= 1.22;
	else if(alpha == 0.05) cAlpha_ 	= 1.36;
	else if(alpha == 0.025) cAlpha_ = 1.48;
	else if(alpha == 0.01) cAlpha_ 	= 1.63;
	else if(alpha == 0.005) cAlpha_ = 1.73;
	else if(alpha == 0.001) cAlpha_ = 1.95;
	else cAlpha_ 					= 1.23;
}