#ifndef _FEATURECAPTURE_H_
#define _FEATURECAPTURE_H_

#include <fstream>
#include <vector>
#include <string>

#include <pcap.h>
#include "../libs/jsoncpp/json/json.h"

#include "PacketCapture.h"


class FeatureTestHandler {
	public:
		FeatureTestHandler(std::fstream *modelFile);
		virtual void initCapture() = 0;
		virtual void JsonToData(Json::Value * json) = 0;
		void loadDataToModel();
		void saveDataToModel();
		virtual void ComputeDistribution(int type) = 0;
		virtual void runTest() = 0;
		virtual int getTestResult() = 0;
		virtual Json::Value *DataToJson() const = 0;
		virtual std::string getFeatureName() const;
		PacketCapture* getPacketCapture() const;
		void addPacketCaptureFile(FILE * pcapFile);
		std::fstream *getModelFile()const;
		
	private:
		PacketCapture *pcap_;
		std::fstream *modelFile_;
	protected:
		static const std::string MODEL_ROOT_;
};
#endif
