///////////////////////////////////////////////////////////////////////////////////
// TopologyTestHandler
// Inherite FeatureTestHandler virtual class and has all information concerning 
// 	the topology test.
///////////////////////////////////////////////////////////////////////////////////

#ifndef _TOPOLOGY_TEST_HANDLER_H_
#define _TOPOLOGY_TEST_HANDLER_H_

#include <fstream> //std::fstream
#include <inttypes.h> //in_addr_t
#include <time.h> // struct timeval
#include <set>


#include <map>

#include "FeatureTestHandler.h"



class TopologyTestHandler : public FeatureTestHandler{
	public:

		TopologyTestHandler(fs::fstream *modelFile, fs::path path, int typeOfTest);
		~TopologyTestHandler();
		void initCapture();
		virtual void computePacket(const struct pcap_pkthdr* pkthdr, const unsigned char * packet );
		virtual void JsonToData(Json::Value * json);
		virtual Json::Value *DataToJson() const ;
		virtual void runTest(double cAlpha);
		virtual int getTestResult();
		void printDistribution() const;
	private:
		std::map<uint64_t,std::set<uint64_t> > *modelTopology_;
		std::map<uint64_t,std::set<uint64_t> > *testTopology_;
		std::map<uint64_t,std::set<uint64_t> > *unknownInteractions_;

};
#endif 	//_TOPOLOGY_TEST_HANDLER_H_
