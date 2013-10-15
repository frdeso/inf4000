#ifndef _FEATURECAPTURE_H_
#define _FEATURECAPTURE_H_

#include <fstream>
#include <vector>
#include <pcap.h>

#include "PacketCapture.h"


class FeatureTestHandler {
	public:
		FeatureTestHandler();
		virtual void initCapture();
		virtual void saveDataToModel();
		PacketCapture* getPacketCapture() const;
		void addPacketCaptureFile(FILE * pcapFile);
		virtual int getTestResult() = 0;
	private:
		PacketCapture *pcap_;
};
#endif
