#ifndef _PACKETLENGTHTESTHANDLER_H_
#define _PACKETLENGTHTESTHANDLER_H_

#include <map>
#include "FeatureTestHandler.h"
#include <stdint.h>
class PacketLengthTestHandler: public FeatureTestHandler
{
	public:
		PacketLengthTestHandler();
		virtual void initCapture();
		virtual void saveDataToModel();
		virtual int getTestResult();	
		void ComputePacketDistribution();
		std::map<uint32_t ,uint32_t>* getPacketDistribution() const;
		void printDistribution() const;
	private:
		std::map <uint32_t,uint32_t> *packetDistribution_;
		size_t maxPacketSize_;
};
#endif
