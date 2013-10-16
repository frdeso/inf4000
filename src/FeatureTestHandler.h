#ifndef _FEATURECAPTURE_H_
#define _FEATURECAPTURE_H_

#include <fstream>
#include <vector>

#include <pcap.h>

#include "PacketCapture.h"


class FeatureTestHandler {
	public:
		FeatureTestHandler(std::fstream *modelFile);
		virtual void initCapture();
		virtual void loadDataToModel() = 0;
		virtual void saveDataToModel() = 0;
		PacketCapture* getPacketCapture() const;
		void addPacketCaptureFile(FILE * pcapFile);
		std::fstream *getModelFile()const;
		virtual int getTestResult() = 0;
	private:
		PacketCapture *pcap_;
		std::fstream *modelFile_;
};
#endif
