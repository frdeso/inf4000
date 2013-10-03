#ifndef _FEATURECAPTURE_H_
#define _FEATURECAPTURE_H_

#include <fstream>
#include <pcap.h>


#include "PacketCapture.h"
class FeatureTestHandler {
	public:
		FeatureTestHandler(FILE * pcap){
			char * error = new char[1000]();
			pcap_ = new PacketCapture(pcap_fopen_offline(pcap, error));
		}
	//	virtual void ReadPacketCapture(std::ifstream *pcap) = 0;
	//	virtual void RunTest() = 0;
		PacketCapture * getPacketCapture() const
		{
			return pcap_;
		}
		void SetPacketCapture(pcap_t * pcap)
		{
			pcap_->setRawData(pcap);
		}
	private:

		PacketCapture * pcap_;
};
#endif
