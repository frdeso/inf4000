#include <pcap.h>
#include <iostream>
#include "PacketLengthTestHandler.h"
int max_size;
void processPacket( unsigned char * arg, const struct pcap_pkthdr* pkthdr, const unsigned char * packet)
{
	if(max_size < pkthdr->len)
		max_size =  pkthdr->len;
}
int main(int argc, char **argv)
{
	FILE *pcap = fopen("../pcap/exp_2-1_clean.pcap", "r") ;
	PacketLengthTestHandler* p  = new PacketLengthTestHandler(pcap);
	
	return 0;
}
