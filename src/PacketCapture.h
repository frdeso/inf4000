///////////////////////////////////////////////////////////////////////////////////
// PacketCapture
// Contains a vector a packet capture files that can be used one after the other
///////////////////////////////////////////////////////////////////////////////////

#ifndef _PACKETCAPTURE_H_
#define _PACKETCAPTURE_H_

#include <vector>

#include <pcap.h>
class PacketCapture{
	public:
		PacketCapture();
		~PacketCapture();
		std::vector<pcap_t*>*	getRawPackets()const;
		void	addRawPacket(pcap_t * pcap);
		size_t getMaxPacketSize() const;
		void setMaxPacketSize(size_t newMax);
	private:
		std::vector<pcap_t*> *rawPackets_;
		size_t maxPacketSize_;
		int nb_packet_;
};
#endif
