#ifndef _TOPOLOGY_TEST_HANDLER_H_
#define _TOPOLOGY_TEST_HANDLER_H_

#include <fstream> //std::fstream
#include <inttypes.h> //in_addr_t
#include <time.h> // struct timeval

#include <map>

#include "FeatureTestHandler.h"

class TopologyTestHandler : public FeatureTestHandler{
	public:

		TopologyTestHandler(fs::fstream *modelFile, fs::path path);
		void initCapture();
		void ComputeDistribution(int type);
		virtual void JsonToData(Json::Value * json);
		virtual Json::Value *DataToJson() const ;
		virtual std::string getFeatureName() const;
		virtual void runTest();
		virtual int getTestResult();
		void printDistribution() const;
	private:
		std::map<uint64_t,uint64_t> topology_;
		static const std::string FEATURE_NAME ;

};
#endif 	//_TOPOLOGY_TEST_HANDLER_H_
