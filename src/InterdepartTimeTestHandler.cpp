
#include <pcap.h>

#include "InterdepartTimeTestHandler.h"
/**
src ;http://homes.di.unimi.it/~gfp/SiRe/2002-03/progetti/libpcap-tutorial.html
*/
void packetDistributionCallback( unsigned char * arg, const struct pcap_pkthdr* pkthdr, const unsigned char * packet )
{
}

InterdepartTimeTestHandler::InterdepartTimeTestHandler(std::fstream *modelFile):FeatureTestHandler(modelFile){}
void InterdepartTimeTestHandler::loadDataToModel(){}
void InterdepartTimeTestHandler::saveDataToModel(){}
int InterdepartTimeTestHandler::getTestResult(){
	//TODO: implemente me
	return 0;
}