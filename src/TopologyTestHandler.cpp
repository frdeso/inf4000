#include "TopologyTestHandler.h"


TopologyTestHandler::TopologyTestHandler(fs::fstream *modelFile, fs::path path):FeatureTestHandler(modelFile, path)
{}
void TopologyTestHandler::initCapture()
{}
void TopologyTestHandler::ComputeDistribution(int type)
{}
void TopologyTestHandler::JsonToData(Json::Value * json)
{}
Json::Value *TopologyTestHandler::DataToJson() const
{
	return (Json::Value *)NULL;
}
std::string TopologyTestHandler::getFeatureName() const{
	return "IMPLEMENT ME";
}
void TopologyTestHandler::runTest(){

}
int TopologyTestHandler::getTestResult(){ 
	return 0;
}
void TopologyTestHandler::printDistribution() const
{

}

const std::string TopologyTestHandler::FEATURE_NAME = "TOPOLOGY";