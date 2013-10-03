#ifndef _PACKETCAPTURE_H_
#define _PACKETCAPTURE_H_

#include <pcap.h>
class PacketCapture{
	public:
		PacketCapture(pcap_t * rawData);
		pcap_t*	getRawData()const;
		void	setRawData(pcap_t * pcap);
		size_t getMaxPacketSize() const;
		void setMaxPacketSize(size_t newMax);
	private:
		pcap_t * rawData_;
		size_t maxPacketSize_;
		int nb_packet_;
};
#endif
