#ifndef _TOPOLOGY_TEST_HANDLER_H_
#define _TOPOLOGY_TEST_HANDLER_H_

#include <fstream> //std::fstream
#include <inttypes.h> //in_addr_t
#include <list> // std::list
#include <map> // std::map
#include <vector> //in_addr_t
#include <tuple> //in_addr_t
#include <time.h> // struct timeval

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

		static const std::string FEATURE_NAME ;

};
#endif 	//_TOPOLOGY_TEST_HANDLER_H_
