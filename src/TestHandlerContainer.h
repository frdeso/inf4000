#ifndef _TEST_HANDLER_CONTAINER_
#define _TEST_HANDLER_CONTAINER_

#include <list>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>


#include "FeatureTestHandler.h"
#include "PacketCapture.h"

class TestHandlerContainer{
public:
	TestHandlerContainer(fs::fstream *modelFile, fs::path path, int type, double alpha);
	void addTestHandler(FeatureTestHandler * handler);
	void addPacketCapture(FILE * pcapFile);
	void loadModel();
	void computeDistribution();
	void saveModel();
	void runTests();
	void printTestsResults();
	void setCAlpha(double alpha);

private:
	PacketCapture *packetCapture_;
	fs::path path_;
	fs::fstream *modelFile_;
	std::list<FeatureTestHandler* > handlers_;
	int type_;
	double cAlpha_;
};

#endif //_TEST_HANDLER_CONTAINER_