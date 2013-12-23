/*
 * FeatureTestHandler
 * Specifying interface for test handlers. Virtual methods must be
 * implemented by the daugther classes.
 * 
 */
 
#ifndef _FEATURECAPTURE_H_
#define _FEATURECAPTURE_H_

#include <fstream>
#include <vector>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include "../libs/jsoncpp/json/json.h"

namespace fs = boost::filesystem ;
class FeatureTestHandler {
	public:
		FeatureTestHandler(fs::fstream *modelFile, fs::path path, int typeOfTest);
		~FeatureTestHandler();
		virtual void initCapture() = 0;
		void loadDataFromModel();
		void saveDataToModel();
		//Gather needed information from packet according to the handler
		virtual void computePacket(const struct pcap_pkthdr* pkthdr, const unsigned char * packet ) = 0;
		//Print the model distribution
		virtual void printDistribution() const = 0;
		// Run the actual statistical test if needed
		virtual void runTest( double cAlpha) = 0;
		// Displays the results of the test
		virtual int getTestResult() = 0;
		// Load the model from the json model file
		virtual void JsonToData(Json::Value * json) = 0;
		// Save the model to the json model file
		virtual Json::Value *DataToJson() const = 0;
		//Return the name of the feature related to this handler
		std::string getFeatureName() const;
		void setFeatureName(std::string name);
		fs::fstream *getModelFile()const;
		void setTypeOfData(int type);
		int getTypeOfData() const;
		
	private:
		fs::path path_;
		fs::fstream *modelFile_;
		std::string FEATURE_NAME;
		int typeOfData_;

	protected:
		static const std::string MODEL_ROOT_;
};
#endif
