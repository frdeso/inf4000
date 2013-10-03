#include "PacketCapture.h"
PacketCapture::PacketCapture(pcap_t * rawData){
	rawData_ = rawData;
}
pcap_t *PacketCapture::getRawData()const{
	return rawData_;
}
void PacketCapture::setRawData(pcap_t *pcap)
{
	rawData_ = pcap;
}

size_t PacketCapture::getMaxPacketSize()const
{
	return maxPacketSize_;
}

void PacketCapture::setMaxPacketSize(size_t newSize)
{
	maxPacketSize_ = newSize;
}
