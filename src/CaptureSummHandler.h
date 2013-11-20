#ifndef _CAPTURE_SUMM_HANDLER_H_
#define _CAPTURE_SUMM_HANDLER_H_

#include <fstream> //std::fstream
#include <inttypes.h> //in_addr_t
#include <time.h> // struct timeval

#include <map>

#include "FeatureTestHandler.h"

class CaptureSummHandler : public FeatureTestHandler{
	public:

		CaptureSummHandler(fs::fstream *modelFile, fs::path path, int typeOfTest);
		~CaptureSummHandler();
		void initCapture();
		virtual void computePacket(const struct pcap_pkthdr* pkthdr, const unsigned char * packet );
		virtual void JsonToData(Json::Value * json);
		virtual Json::Value *DataToJson() const ;
		virtual void runTest(double cAlpha);
		virtual int getTestResult();
		void printDistribution() const;
	private:
		uint32_t nbOfPackets_;
		uint64_t beginning_;
		uint64_t duration_;

};
#endif 	//_CAPTURE_SUMM_HANDLER_H_
