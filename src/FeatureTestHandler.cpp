#include "FeatureTestHandler.h"
#include <iostream>
#include <boost/filesystem/fstream.hpp>
	
using namespace std;

FeatureTestHandler::FeatureTestHandler(fs::fstream * modelFile, fs::path path){
	pcap_ = new PacketCapture();
	modelFile_ = modelFile;
	path_ = path;
}

PacketCapture *FeatureTestHandler::getPacketCapture() const
{
	return pcap_;
}
fs::fstream *FeatureTestHandler::getModelFile()const{
	return modelFile_;
}
void FeatureTestHandler::addPacketCaptureFile(FILE * pcapFile)
{
	char error[PCAP_ERRBUF_SIZE];
	
	pcap_t* p = pcap_fopen_offline(pcapFile, error);

	if(p == NULL) {
		cerr<<error<<endl;
	} else {
		this->getPacketCapture()->addRawPacket(p);
	}
}

void FeatureTestHandler::saveDataToModel(){
	Json::Value *vec = DataToJson();
	Json::Value oldroot;
	getModelFile()->seekg(0, getModelFile()->beg);
	
	Json::Reader reader;
	if(fs::is_empty(path_)) { 
		cout  << "Empty model file." << endl;
	} else {
		bool parsingSuccessful = reader.parse( (*getModelFile()), oldroot, false );

		if(oldroot.size() == 0 || oldroot[MODEL_ROOT_].isNull()) {
			cerr  << "File not empty, but no model found" << endl;
		} else if ( !parsingSuccessful ) {
			cerr  <<"Parsing error :" <<reader.getFormatedErrorMessages() << endl;
			return;
		} 
	}
getModelFile()->clear();
	//Take the feature section of the model
	oldroot[MODEL_ROOT_][getFeatureName()] = *vec;
	getModelFile()->seekg(0, getModelFile()->beg);
	(*getModelFile()) << oldroot;
	delete vec;
	return;
}

void FeatureTestHandler::loadDataToModel(){
	Json::Value *root = new Json::Value();
	Json::Reader reader;

	getModelFile()->seekg(0, ios::beg);
	bool parsingSuccessful = reader.parse( (*getModelFile()), (*root), false );

 	if ( !parsingSuccessful ) {
		cerr  <<"Parsing error :" <<reader.getFormatedErrorMessages() << "File not empty, but no model found."<<endl;
		throw std::ios_base::failure("Parsing unsuccesful.");
		return;
	} else{
		JsonToData(root);
	}
	delete root;

	return;
}


string FeatureTestHandler::getFeatureName() const{
	return "FEATURE_TEST_HANDLER";
}


const string FeatureTestHandler::MODEL_ROOT_ = "model";