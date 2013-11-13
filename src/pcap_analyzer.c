#include <iostream>
#include <algorithm>
#include <errno.h>
#include <cstring>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <pcap.h>
#include "../libs/jsoncpp/json/json.h"

#include "InterdepartTimeTestHandler.h"
#include "PacketLengthTestHandler.h"
#include "TopologyTestHandler.h"

#include "constants.h"
unsigned int  max_size;

namespace fs = boost::filesystem ;
//using namespace fs;
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
		std::cerr<<"Display help menu"<<std::endl;
		return -1;
	} 
	/* Check for conflicting arguments */
	if (cmdOptionExists(argv, argv + argc, "-l") && cmdOptionExists(argv, argv + argc, "-a")) {
		std::cerr<<"You specified two conflicting arguments '-l' and '-a'. Please specify only one of them." <<std::endl;
		return -1;
	}
		/* Check for missing arguments */
	if (!cmdOptionExists(argv, argv + argc, "-l") && !cmdOptionExists(argv, argv + argc, "-a")) {
		std::cerr<<"You did not specified any argument. Please specify the type of data you have." <<std::endl;
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
		std::cerr<<"No '-f' argument."<<std::endl;
		return -1;
	}

	/*Beginning of the program*/
	FILE *pcap = fopen(pathToFile_.c_str(), "r");
	if(pcap == NULL) {
		std::cerr << "Trouble openning the file... "<<std::strerror(errno)<<std::endl;	
		return -1;	
	}

	fs::path pathToModel("model.json");
	if(!fs::exists(pathToModel)){
		if(typeOfData == LEARNING_DATA) {
			std::cerr<<"Model file does not seems to exist or is empty. Creating one..."<<std::endl;
		}
		else if(typeOfData == ANALYSIS_DATA){
			std::cerr<<"Model file needed for analysis but not found or is empty. Exiting."<<std::endl;
			return -1;
		}
	}
	fs::fstream *modelFile = new fs::fstream();
	
	FeatureTestHandler* p  = new InterdepartTimeTestHandler(modelFile,pathToModel);
//	FeatureTestHandler* p  = new PacketLengthTestHandler(modelFile);
//	TopologyTestHandler *p = new TopologyTestHandler(modelFile,pathToModel);
	p->addPacketCaptureFile(pcap);
	try{
		modelFile->open(pathToModel, std::ios::in);
		modelFile->exceptions ( std::ios::failbit | std::ios::badbit );

		p->loadDataToModel();
	}catch(std::ifstream::failure e){
		if( typeOfData == ANALYSIS_DATA){
			std::cerr<<"Exception during loading of the model.";
			std::cerr<<" Exception type: "<<e.what()<<std::endl;
			return -1;
		}
		else{
			modelFile->clear(); // to reset the fail and bad flags
		}
		
	}
	//modelFile->clear();
	
	p->ComputeDistribution(typeOfData);
	p->initCapture();
	if(typeOfData == LEARNING_DATA){
		modelFile->close();
		modelFile->open(pathToModel, std::ios::out |std::ios::in);
		modelFile->exceptions ( std::ios::failbit | std::ios::badbit );

		try{
			
			p->saveDataToModel();
	}catch(std::ifstream::failure e){
		std::cerr<<"Exception during the saving of the data. Exception type: "<<e.what()<<std::endl;
		
		return -1;
	}
	}else if(typeOfData == ANALYSIS_DATA){
		p->runTest();
		p->getTestResult();
	}
	std::cout.flush();

	modelFile->close();
	return 0;
}
