#ifndef _PACKETLENGTHTESTHANDLER_H_
#define _PACKETLENGTHTESTHANDLER_H_



#include "FeatureTestHandler.h"
class PacketLengthTestHandler: public FeatureTestHandler
{
	public:
		PacketLengthTestHandler(FILE * fichier);
		virtual void ReadPacketCapture(pcap_t * pcap);
		virtual void RunTest();

		size_t ComputeMaxPacketSize();
	private:

		size_t maxPacketSize_;
};
#endif
