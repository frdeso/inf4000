///////////////////////////////////////////////////////////////////////////////////
// TestHandlerContainer
// Contain a list of feature test handler. For each stage of the test this conatainer
// calls the corresponding method in each handler.
///////////////////////////////////////////////////////////////////////////////////

#ifndef _TEST_HANDLER_CONTAINER_
#define _TEST_HANDLER_CONTAINER_

#include <list>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <pcap.h>

#include "FeatureTestHandler.h"
#include "PacketCapture.h"

class TestHandlerContainer{
public:
	TestHandlerContainer(fs::fstream *modelFile, fs::path path, int type, double alpha);
	void addTestHandler(FeatureTestHandler * handler);
	void addPacketCapture(pcap_t * captureHandle);
	void loadModel();
	void computeDistribution(bool isLive, uint32_t duration);
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