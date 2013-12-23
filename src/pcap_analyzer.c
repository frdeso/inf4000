#include <algorithm>
#include <cstring>
#include <errno.h>
#include <iostream>
#include <stdlib.h>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <pcap.h>
#include "../libs/jsoncpp/json/json.h"

#include "CaptureSummHandler.h"
#include "InterdepartTimeTestHandler.h"
#include "PacketLengthTestHandler.h"
#include "TestHandlerContainer.h"
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

void printUsage()
{
	std::cout<<"Usage: pcap_analyzer [MODE] [SOURCE] [SOURCE-OPTION]" <<std::endl;
	std::cout<<"You need to specify either of these two modes." <<std::endl;
	std::cout<<"\t-l\t\t Learning mode." <<std::endl;
	std::cout<<"\t-a\t\t Analysis mode." <<std::endl;

	std::cout<<"You need to specify either of these two sources plus one option." <<std::endl;
	std::cout<<"\t-f FILE\t\t Read packets from a packet capture file." <<std::endl;
	std::cout<<"\t-o INTERFACE\t Read packets live from an interface. (May need root privileges)" <<std::endl;

	std::cout<<"Optional flags" <<std::endl;
	std::cout<<"\t-d DURATION\t Specify duration in seconds of the live capture." <<std::endl;
	std::cout<<"\t-c VALUE\t Specify critical value to use in analysis mode." <<std::endl;
	std::cout<<"" <<std::endl;
}


/*
 * Available options : -h : help, -l : learning mode, -a :analysis mode, -f <path to file> pcap file to learning from or to analyze
 */

int main(int argc, char **argv)
{
	int typeOfData = LEARNING_DATA;
	double criticalValue = 0.10;
	bool isLive = true;
	uint32_t duration = 10; //default duration for live capture
	pcap_t* captureHandler;
	std::string pathToFile_;
	std::string interface;

	if(cmdOptionExists(argv, argv + argc, "-h")){
		/*TODO: help menu*/
		printUsage();
		return -1;
	} 
	/* Check for conflicting arguments */
	if (cmdOptionExists(argv, argv + argc, "-l") && cmdOptionExists(argv, argv + argc, "-a")) {
		std::cerr<<"You specified two conflicting arguments '-l' and '-a'. Please specify only one of them." <<std::endl;
		printUsage();
		return -1;
	}
		/* Check for missing arguments */
	if (!cmdOptionExists(argv, argv + argc, "-l") && !cmdOptionExists(argv, argv + argc, "-a")) {
		std::cerr<<"You did not specified any argument. Please specify the type of data you have." <<std::endl;
		printUsage();
		return -1;
	}


	if ( cmdOptionExists(argv, argv + argc, "-l") ){
		std::cout<<"Learning mode."<<std::endl;
		typeOfData = LEARNING_DATA;
	} else if ( cmdOptionExists(argv, argv + argc, "-a") ){
		std::cout<<"Analysis mode."<<std::endl;
		typeOfData = ANALYSIS_DATA;
		if (!cmdOptionExists(argv, argv + argc, "-c")) {
			std::cerr<<"You did not specified a critical value. Default value("<<criticalValue<<") will be used." <<std::endl;
		}else{
			criticalValue =  atof(getCmdOption(argv, argv + argc, "-c"));
		}
	}

	/* Check for conflicting arguments */
	if (cmdOptionExists(argv, argv + argc, "-f") && cmdOptionExists(argv, argv + argc, "-o")) {
		std::cerr<<"You specified two conflicting source arguments '-f' and '-o'. Please specify only one of them." <<std::endl;
		printUsage();
		return -1;
	}
		/* Check for missing arguments */
	if (!cmdOptionExists(argv, argv + argc, "-f") && !cmdOptionExists(argv, argv + argc, "-o")) {
		std::cerr<<"You did not specified any source argument. Please specify online(\"-o\") or offline(\"-f\")." <<std::endl;
		printUsage();
		return -1;
	}

	if(cmdOptionExists(argv, argv + argc, "-o")){
		isLive = true;
		interface = std::string(getCmdOption(argv, argv + argc, "-o"));
		if(cmdOptionExists(argv, argv + argc, "-d")){
			duration = atoi(getCmdOption(argv, argv + argc, "-d"));
		}
	}else if(cmdOptionExists(argv, argv + argc, "-f")){
		isLive = false;
		pathToFile_ = std::string(getCmdOption(argv, argv + argc, "-f"));
	}

	/*Beginning of the program*/

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
	
	TestHandlerContainer *container = new TestHandlerContainer(modelFile, pathToModel, typeOfData, criticalValue);
	container->addTestHandler(new InterdepartTimeTestHandler(modelFile,pathToModel, typeOfData)	);

	container->addTestHandler(new PacketLengthTestHandler(modelFile,pathToModel,typeOfData)	);
	
	container->addTestHandler(new TopologyTestHandler(modelFile,pathToModel,typeOfData)		);
	
	//container->addTestHandler(new CaptureSummHandler(modelFile,pathToModel,typeOfData)		);
	
	char error[PCAP_ERRBUF_SIZE];
	if(isLive){
		pcap_if_t *devices, *first;
     	if(pcap_findalldevs(&devices,error )){
     		printf("Trouble finding all devices. Exiting.\n");
     		return -1;
     	}
     	first = devices;
     	bool interfaceFound = false;
		while(devices != NULL){
        	if(strcmp(devices->name, interface.c_str()) == 0){
        		interfaceFound = true;
        		break;
        	}
        	devices = devices->next;
     	}
     	if(interfaceFound == false){
     		std::cerr<<"Specified interface not found. ";
     		if(first != NULL) std::cerr<<"Here is a list of the interfaces:"<<std::endl;
     		else std::cerr<<"You may need root privileges."<<std::endl;
     		while(first != NULL){
        		//printf("%s\n", first->description);
        		std::cerr<<"\t- "<<first->name<<std::endl;
        		first = first->next;
     		}
     		return -1;
     	}

		captureHandler = pcap_open_live(interface.c_str(),BUFSIZ,0,-1,error);
		std::cout<<"Starting live capture on interface "<< interface<<" for "<<duration<<" second(s)...";
	}
	else {
		FILE *pcap = fopen(pathToFile_.c_str(), "r");
		if(pcap == NULL) {
			std::cerr << "Trouble openning the capture file... "<<std::strerror(errno)<<std::endl;	
			return -1;	
		}
		captureHandler = pcap_fopen_offline(pcap, error);
		std::cout<<"Reading the capture file "<<pathToFile_<<" ...";
		if(captureHandler == NULL) {
			std::cerr<<"Error openning the pcap file: "<<error<<std::endl;
			exit(-1);
		}
	}


	container->addPacketCapture(captureHandler);
	try{
		modelFile->open(pathToModel, std::ios::in);
		modelFile->exceptions ( std::ios::failbit | std::ios::badbit );

		container->loadModel();
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
	std::cout<<std::flush;
	container->computeDistribution(isLive, duration);
	std::cout<<" Done."<<std::endl;
	//p->initCapture();
	if(typeOfData == LEARNING_DATA){
		modelFile->close();
		modelFile->open(pathToModel, std::ios::out |std::ios::in);
		modelFile->exceptions ( std::ios::failbit | std::ios::badbit );

		try{
			
			container->saveModel();
		}catch(std::ifstream::failure e){
			std::cerr<<"Exception during the saving of the data. Exception type: "<<e.what()<<std::endl;
			return -1;
		}
		std::cout<<"Done learning from this capture."<< std::endl;
	}
	else if(typeOfData == ANALYSIS_DATA){
		std::cout<<"Testing capture against model..."<<std::endl;
		container->runTests();
		container->printTestsResults();
	}
	std::cout.flush();

	modelFile->close();
	return 0;
}
