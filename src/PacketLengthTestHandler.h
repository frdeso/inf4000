#ifndef _PACKETLENGTHTESTHANDLER_H_
#define _PACKETLENGTHTESTHANDLER_H_

#include <map>
#include <utility>
#include "FeatureTestHandler.h"

class PacketLengthTestHandler: public FeatureTestHandler
{
	public:
		PacketLengthTestHandler(FILE * fichier);
		virtual void ReadPacketCapture(pcap_t * pcap);
		virtual void RunTest();

		size_t ComputeMaxPacketSize();
		std::map<unsigned int , unsigned int>* getPacketDistribution() const;
		void printDistribution() const;
	private:
		std::map <unsigned int, unsigned int> *packetDistribution_;
		size_t maxPacketSize_;
};
#endif
