#ifndef _PACKETLENGTHTESTHANDLER_H_
#define _PACKETLENGTHTESTHANDLER_H_

#include <map>
#include <fstream>
#include <stdint.h>

#include "FeatureTestHandler.h"

class PacketLengthTestHandler: public FeatureTestHandler
{
	public:
		PacketLengthTestHandler(std::fstream *modelFile);
		virtual void initCapture();
		virtual void loadDataToModel();
		virtual void saveDataToModel();
		void runTest();
		virtual int getTestResult();	
		void ComputeDistribution(int type);
		std::map<uint32_t ,uint32_t>* getModelDistribution() const;
		std::map<uint32_t ,uint32_t>* getTestDistribution() const;
		void computeModelMaxValue();
		void computeTestMaxValue();
		void printDistribution() const;
	private:
		uint32_t getModelSampleSize();
		uint32_t getTestSampleSize();
		std::map <uint32_t,uint32_t> *modelDist_;
		std::map <uint32_t,uint32_t> *testDist_;
		std::vector<double> *modelCumulDist_;
		std::vector<double> *testCumulDist_;
		size_t maxModelSize_;
		double dStat_;
		size_t maxTestSize_;
		static const double BUFFER = 0.000000000001; // 1*10^-15
};
#endif
