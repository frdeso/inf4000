#include <iostream>
#include <algorithm>
#include <errno.h>
#include <cstring>

#include <pcap.h>
#include "../libs/jsoncpp/json/json.h"

#include "PacketLengthTestHandler.h"
unsigned int  max_size;

enum TYPE_OF_DATA{LEARNING_DATA, ANALYSIS_DATA, NB_ACTIONS};

/*
source: http://stackoverflow.com/questions/865668/parse-command-line-arguments
*/
char* getCmdOption(char ** begin, char ** end, const std::string & option)
{
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

/*
source: http://stackoverflow.com/questions/865668/parse-command-line-arguments
*/
bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}

/*
 * Available options : -h : help, -l : learning mode, -a :analysis mode, -f <path to file> pcap file to learning from or to analyze
 *
 */

int main(int argc, char **argv)
{
	TYPE_OF_DATA typeOfData_;
	std::string pathToFile_;

	if(cmdOptionExists(argv, argv + argc, "-h")){
		/*TODO: help menu*/
		std::cout<<"Display help menu"<<std::endl;
		return -1;
	} 
	/* Check for conflicting arguments */
	if (cmdOptionExists(argv, argv + argc, "-l") && cmdOptionExists(argv, argv + argc, "-a")) {
		std::cout<<"You specified two conflicting arguments '-l' and '-a'. Please specify only one of them." <<std::endl;
		return -1;
	}
		/* Check for missing arguments */
	if (!cmdOptionExists(argv, argv + argc, "-l") && !cmdOptionExists(argv, argv + argc, "-a")) {
		std::cout<<"You did not specified any argument. Please specify the type of data you have." <<std::endl;
		return -1;
	}

	if ( cmdOptionExists(argv, argv + argc, "-l") ){
		typeOfData_ = LEARNING_DATA;
	} else if ( cmdOptionExists(argv, argv + argc, "-a") ){
		typeOfData_ = ANALYSIS_DATA;
	}
	if(cmdOptionExists(argv, argv + argc, "-f")){
		pathToFile_ = std::string(getCmdOption(argv, argv + argc, "-f"));
	}else{
		std::cout<<"No '-f' argument."<<std::endl;
		return -1;
	}

	/*Beginning of the program*/
	FILE *pcap = fopen(pathToFile_.c_str(), "r");
	if(pcap == NULL) {
		std::cerr << "Trouble openning the file... "<<std::strerror(errno)<<std::endl;	
		return -1;	
	}
	std::fstream *modelFile = new std::fstream();
	modelFile->open("model.json",std::ios::in | std::ios::out );
	if(!modelFile->is_open()) {
		std::cout<<"Model file does not seems to exist. Creating one..."<<std::endl;
	}
		
	//*modelFile << "allol";
	PacketLengthTestHandler* p  = new PacketLengthTestHandler(modelFile);
	p->addPacketCaptureFile(pcap);
	p->initCapture();
	p->printDistribution();
	p->saveDataToModel();
	
	modelFile->close();
	return 0;
}
