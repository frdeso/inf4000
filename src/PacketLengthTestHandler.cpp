#include <iostream>
#include <map>
#include <math.h>
#include <iomanip> //setw()
#include <limits.h>

#include "../libs/jsoncpp/json/json.h"

#include "constants.h"
#include "PacketLengthTestHandler.h"
#include "PacketCapture.h"
#include "utils.h"


using namespace std;
PacketLengthTestHandler::PacketLengthTestHandler(fs::fstream *modelFile, fs::path path, int typeOfTest):FeatureTestHandler(modelFile, path, typeOfTest)
{
	modelDist_ = new map<uint32_t,uint32_t>();
	testDist_ = new map<uint32_t,uint32_t>();
	modelCumulDist_ = new vector<double>();
	testCumulDist_ = new vector<double>();
	maxModelSize_ = 0;
	maxTestSize_ = 0;
	dStat_ = 0;
	setFeatureName("PACKET_LENGHT");
}

PacketLengthTestHandler::~PacketLengthTestHandler(){}

void PacketLengthTestHandler::initCapture(){
	computeModelMaxValue();
	computeTestMaxValue();
}
void PacketLengthTestHandler::JsonToData(Json::Value * root){
	//Take the feature section of the model
	Json::Value featureName = (*root)[MODEL_ROOT_][getFeatureName()]; //FIXME: understand what can i do with the "null" situation

	//Add data
	for ( unsigned int i = 0; i < featureName.size(); i++ ) {
		//cout << "Loading of model data : " << featureName[i][0].asUInt() <<":"<< featureName[i][1].asUInt() <<endl;
		uint32_t packetlength = featureName[i][0].asUInt();
		uint32_t currentNumber = featureName[i][1].asUInt();
		(*getModelDistribution())[packetlength] += currentNumber;
	}

}

Json::Value* PacketLengthTestHandler::DataToJson() const
{
	Json::Value *vect = new Json::Value(Json::arrayValue);

	for ( map<uint32_t ,uint32_t>::iterator it=getModelDistribution()->begin(); it!=getModelDistribution()->end(); ++it){
		Json::Value subVec(Json::arrayValue);
		subVec.append(it->first);
		subVec.append(it->second);

		vect->append(subVec);
	}
	return vect;
}

void PacketLengthTestHandler::computePacket(const struct pcap_pkthdr* pkthdr, const unsigned char * packet ){

	map<uint32_t ,uint32_t> * arg;
	if(getTypeOfData() == LEARNING_DATA) arg = getModelDistribution();
	else if(getTypeOfData() == ANALYSIS_DATA) arg = getTestDistribution();

	map<uint32_t,uint32_t>* p = (map<uint32_t,uint32_t>* ) arg;

	(*p)[pkthdr->len]++;

}


map<uint32_t ,uint32_t>* PacketLengthTestHandler::getModelDistribution() const
{
	return modelDist_;
}

map<uint32_t ,uint32_t>* PacketLengthTestHandler::getTestDistribution() const
{
	return testDist_;
}

void PacketLengthTestHandler::printDistribution() const
{
	for ( map<uint32_t ,uint32_t>::iterator it=getModelDistribution()->begin(); it!=getModelDistribution()->end(); ++it)
		cout << it->first << "\t=> " << it->second <<  endl;
}

int PacketLengthTestHandler::getTestResult(){
	//TODO: take alpha by argument or set default in a better place
	
	if(dStat_ > seuil_)
		cout << RED<<"The test distribution does not match the model."<<NO_COLOR<< "(dStat_: "<<dStat_<< ", seuil:"<<seuil_<<")"<<endl;
	else
		cout << GREEN<<"The test distribution matches with the model."<<NO_COLOR<<endl;
	return 0;
}

void PacketLengthTestHandler::runTest(double cAlpha){
	cAlpha_ = cAlpha;
	//compute cumulative function of the model
	double lastSum = 0;
	uint32_t numberOfElementModel = getModelSampleSize();

	//cout<<"numberOfElementModel: "<< numberOfElementModel<<endl;
	for(uint32_t i = 0; i <= maxModelSize_; ++i) {
		modelCumulDist_->push_back(lastSum + ((*getModelDistribution())[i])/(double)numberOfElementModel);
		//cout<<", [i,j]:"<<i<<","<<(*getModelDistribution())[i]<<endl;
		lastSum = (*modelCumulDist_)[i];
		//cout<<"LS: "<<lastSum<<endl;
		if ((*getModelDistribution())[i] <= 0)
			//FIXME:Ghetto!! maybe change data structure. 
			getModelDistribution()->erase(i);
	}

	//compute cumulative function of the test 
	lastSum = 0;
	uint32_t numberOfElementTest = getTestSampleSize();

	for(uint32_t i = 0; i <= maxTestSize_; ++i) {
		testCumulDist_->push_back(lastSum + ((*getTestDistribution())[i])/(double)numberOfElementTest);
		lastSum = (*testCumulDist_)[i];
		if ((*getTestDistribution())[i] <= 0)
			//FIXME:Ghetto!! maybe change data structure. 
			getTestDistribution()->erase(i);
	}

	double modelValue = 0;
	double testValue = 0;
	
	uint32_t max =  (maxModelSize_ > maxTestSize_ ? maxModelSize_ : maxTestSize_ );
	for(uint32_t i = 0; i <= max ; ++i){
		
		modelValue = (i >= maxModelSize_ ? 1:  (*modelCumulDist_)[i]);

		testValue = (i >= maxTestSize_ ? 1:  (*testCumulDist_)[i]);

		//cout<<"i: " << setw(3)<<i <<", model: "<<setw(9)<<modelValue<< ", test: "<<setw(9)<< testValue <<", test - model: "<<setw(9)<<fabs(testValue - modelValue)<<endl;

		if (dStat_ < fabs(testValue - modelValue)){
			dStat_ = fabs(testValue - modelValue);
		}

	}

	double nModel = (double ) numberOfElementModel;
	double nTest =  (double) numberOfElementTest;

	seuil_ = cAlpha*pow((nModel + nTest)/(nModel * nTest), 0.5);
}

void PacketLengthTestHandler::computeModelMaxValue()
{
	for ( map<uint32_t ,uint32_t>::iterator it=getModelDistribution()->begin(); it!=getModelDistribution()->end(); ++it)
		if(maxModelSize_ < it->first) {
			maxModelSize_ = it->first;
		}
}

void PacketLengthTestHandler::computeTestMaxValue()
{
	for ( map<uint32_t ,uint32_t>::iterator it=getTestDistribution()->begin(); it!=getTestDistribution()->end(); ++it)
		if(maxTestSize_ < it->first) {
			maxTestSize_ = it->first;
		}
}

uint32_t PacketLengthTestHandler::getModelSampleSize()
{
	static uint32_t size = UINT_MAX;
	if(size == UINT_MAX){
		size = 0;
		for ( map<uint32_t ,uint32_t>::iterator it=getModelDistribution()->begin(); it!=getModelDistribution()->end(); ++it){
			size += it->second;
		}
	}
	return size;
}

uint32_t PacketLengthTestHandler::getTestSampleSize()
{
	static uint32_t size = UINT_MAX;
	if(size == UINT_MAX){
		size = 0;
		for ( map<uint32_t ,uint32_t>::iterator it=getTestDistribution()->begin(); it!=getTestDistribution()->end(); ++it){
			size += it->second;
		}
	}
	return size;
}

const double PacketLengthTestHandler::BUFFER = 0.000000000001;