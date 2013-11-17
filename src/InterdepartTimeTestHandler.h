#ifndef _INTERDEPART_TIME_TEST_HANDLER_H_
#define _INTERDEPART_TIME_TEST_HANDLER_H_

#include <fstream> //std::fstream
#include <inttypes.h> //in_addr_t
#include <list> // std::list
#include <map> // std::map
#include <vector> //in_addr_t
#include <tuple> //in_addr_t
#include <time.h> // struct timeval

#include "FeatureTestHandler.h"

class InterdepartTimeTestHandler : public FeatureTestHandler{
	public:

		InterdepartTimeTestHandler(fs::fstream *modelFile, fs::path path, int typeOfTest);
		~InterdepartTimeTestHandler();
		void initCapture();
		virtual void computePacket(const struct pcap_pkthdr* pkthdr, const unsigned char * packet );
		virtual void JsonToData(Json::Value * json);
		virtual Json::Value *DataToJson() const ;
		virtual void runTest();
		virtual int getTestResult();
		void printDistribution() const;
	private:
		void ComputeInterdeparture(std::map<uint32_t, std::list<uint64_t> > *packetTiming, std::map<uint32_t, std::map<uint64_t, uint32_t> > * timing);
		double findValueInCumul(std::vector<std::tuple<uint64_t,uint64_t, double> > cumul, uint64_t value);
		uint32_t computeNumElement(std::map<uint64_t, uint32_t> *dist);
		uint64_t computeMaxTiming(std::map<uint64_t, uint32_t> *dist);
		std::list<std::tuple<uint64_t, double, double> > dStats_;
		std::map<uint32_t, std::map<uint64_t, uint32_t> > * interdepTiming_; //TODO::change to modelInterdepTiming_
		std::map<uint32_t, std::map<uint64_t, uint32_t> > * testInterdepTiming_;
		std::map<uint32_t, std::vector<uint64_t> > * interdepTimingCumul_;
		std::map<uint32_t, std::list<uint64_t> > * packetTiming_;
};

#endif // _INTERDEPART_TIME_TEST_HANDLER_H_