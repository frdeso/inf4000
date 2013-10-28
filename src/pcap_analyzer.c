#include <iostream>
#include <algorithm>
#include <errno.h>
#include <cstring>

#include <pcap.h>
#include "../libs/jsoncpp/json/json.h"

#include "InterdepartTimeTestHandler.h"
#include "PacketLengthTestHandler.h"


#include "constants.h"
unsigned int  max_size;

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

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}

/*
 * Available options : -h : help, -l : learning mode, -a :analysis mode, -f <path to file> pcap file to learning from or to analyze
 */

int main(int argc, char **argv)
{
	int typeOfData;
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
		typeOfData = LEARNING_DATA;
	} else if ( cmdOptionExists(argv, argv + argc, "-a") ){
		typeOfData = ANALYSIS_DATA;
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
	modelFile->seekg(0, std::ios::end);
	modelFile->exceptions ( std::ifstream::failbit | std::ifstream::badbit );
	if(!modelFile->is_open() || modelFile->tellg() <= 0){
		if(typeOfData == LEARNING_DATA) {
			modelFile->seekg(0, std::ios::beg);
			std::cout<<"Model file does not seems to exist or is empty. Creating one..."<<std::endl;
		}
		else if(typeOfData == ANALYSIS_DATA){
			std::cerr<<"Model file needed for analysis but not found or is empty. Exiting."<<std::endl;
			return -1;
		}
	}
		
//	FeatureTestHandler* p  = new InterdepartTimeTestHandler(modelFile);
	FeatureTestHandler* p  = new PacketLengthTestHandler(modelFile);
	p->addPacketCaptureFile(pcap);
	try{
		p->loadDataToModel();
	}catch(std::ifstream::failure e){
		std::cerr<<"Exception during loading of the model.";
		if( typeOfData == ANALYSIS_DATA){
			std::cerr<<" Exception type: "<<e.what()<<std::endl;
			return -1;
		}
		else{
			modelFile->clear(); // to reset the fail and bad flags
		}
		
	}
	p->ComputeDistribution(typeOfData);
	p->initCapture();
	if(typeOfData == LEARNING_DATA){
		try{
		p->saveDataToModel();
	}catch(std::ifstream::failure e){
		std::cout<<"Exception during the saving of the data. Exception type: "<<e.what()<<std::endl;
		
		return -1;
	}
	}else if(typeOfData == ANALYSIS_DATA){
		p->runTest();
		p->getTestResult();
	}
	
	modelFile->close();
	return 0;
}
