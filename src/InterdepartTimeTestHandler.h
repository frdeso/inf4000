#ifndef _INTERDEPART_TIME_TEST_HANDLER_H_
#define _INTERDEPART_TIME_TEST_HANDLER_H_

#include <fstream> //std::fstream

#include "FeatureTestHandler.h"

class InterdepartTimeTestHandler : public FeatureTestHandler{
	public:
		InterdepartTimeTestHandler(std::fstream *modelFile);
		virtual void loadDataToModel();
		virtual void saveDataToModel();
		virtual int getTestResult();
	private:


};

#endif // _INTERDEPART_TIME_TEST_HANDLER_H_