#include "FeatureTestHandler.h"
#include <iostream>
#include <boost/filesystem/fstream.hpp>
	
using namespace std;

FeatureTestHandler::FeatureTestHandler(fs::fstream * modelFile, fs::path path, int typeOfData){
	modelFile_ = modelFile;
	path_ = path;
	typeOfData_ = typeOfData;
}
FeatureTestHandler::~FeatureTestHandler(){
}

fs::fstream *FeatureTestHandler::getModelFile()const{
	return modelFile_;
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

void FeatureTestHandler::loadDataFromModel(){
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
	return FEATURE_NAME;
}
void FeatureTestHandler::setFeatureName(string name){
	FEATURE_NAME = name;
}

void FeatureTestHandler::setTypeOfData(int type){
	typeOfData_ = type;
}
int FeatureTestHandler::getTypeOfData() const{
	return typeOfData_;
}


const string FeatureTestHandler::MODEL_ROOT_ = "model";