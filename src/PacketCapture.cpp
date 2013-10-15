#include "PacketCapture.h"
using namespace std;

PacketCapture::PacketCapture(){
	rawPackets_ = new vector<pcap_t*>();
}
PacketCapture::~PacketCapture(){
	/*FIXME: Possible memory leak. Since the pcap_t are most likely allocated
	 * with malloc those cannot be freed with delete used by
	 * std::vector::clear()*/
	rawPackets_->clear();
	delete rawPackets_;
}
std::vector<pcap_t*> *PacketCapture::getRawPackets()const{
	return rawPackets_;
}
void PacketCapture::addRawPacket(pcap_t *pcap)
{
	rawPackets_->push_back(pcap);
}

size_t PacketCapture::getMaxPacketSize()const
{
	return maxPacketSize_;
}

void PacketCapture::setMaxPacketSize(size_t newSize)
{
	maxPacketSize_ = newSize;
}
