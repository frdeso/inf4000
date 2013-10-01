#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>

void processPacket( unsigned char * arg, const struct pcap_pkthdr* pkthdr, const unsigned char * packet)
{
	printf("packet_len: %d \n", pkthdr->len);
}
int main(int argc, char **argv)
{
	FILE * fichier;
	char * errbuf = malloc(sizeof(char)*1000);
	pcap_t * paquet = pcap_open_offline("../pcap/exp_2-1_clean.pcap", errbuf);

	int count = 0;
	pcap_loop(paquet, -1, processPacket, (unsigned char *) &count );
	
	return 0;
}
