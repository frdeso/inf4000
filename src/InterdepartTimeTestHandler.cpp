#include <arpa/inet.h>
#include <cfloat>
#include <climits>//UINT_MAX
#include <iostream>
#include <iomanip> //setw()
#include <math.h> //fabs
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <stdio.h> //scanf
#include <sstream> // for ostringstream

#include "../libs/jsoncpp/json/json.h"
#include <pcap.h>


#include "constants.h"
#include "InterdepartTimeTestHandler.h"
#include "utils.h"


using namespace std;

InterdepartTimeTestHandler::InterdepartTimeTestHandler(fs::fstream *modelFile, fs::path path, int typeOfTest):FeatureTestHandler(modelFile, path, typeOfTest){
	interdepTiming_ 		= new map<uint32_t, map<uint64_t, uint32_t> >();
	testInterdepTiming_		= new map<uint32_t, map<uint64_t, uint32_t> >();
	interdepTimingCumul_ 	= new map<uint32_t, vector<uint64_t> >();
	packetTiming_ 			= new map<uint32_t, list<uint64_t> >();

	setFeatureName("INTERDEPARTURE_TIME");

}
InterdepartTimeTestHandler::~InterdepartTimeTestHandler(){}

void InterdepartTimeTestHandler::computePacket(const struct pcap_pkthdr* pkthdr, const unsigned char * packet ){

	struct ip * ipHdr = (struct ip*) (packet + sizeof(struct ether_header));

	(*packetTiming_)[ipHdr->ip_src.s_addr].push_back(timevalToUINT64(pkthdr->ts));
}


void InterdepartTimeTestHandler::initCapture(){
	map<uint32_t, std::map<uint64_t, uint32_t> > * timing;
	if(getTypeOfData() == LEARNING_DATA) timing = interdepTiming_;
	else if(getTypeOfData() == ANALYSIS_DATA) timing = testInterdepTiming_;
	ComputeInterdeparture(packetTiming_, timing);
	
}

void InterdepartTimeTestHandler::JsonToData(Json::Value * root){


	//Take the feature section of the model
	Json::Value featureName = (*root)[MODEL_ROOT_][getFeatureName()]; //FIXME: understand what can i do with the "null" situation
	std::vector<string> addressList = featureName.getMemberNames();
	//cout <<"size: "<< addressList.size()<<endl;
	//Add data
	  for (std::vector<string>::iterator it = addressList.begin() ; it != addressList.end(); ++it){
	  	//cout<<*it<<endl;
		for(uint32_t i = 0; i < featureName[it->c_str()].size(); ++i){
			uint32_t address = atoi(it->c_str());
			
			//cout<<featureName[*it][i][1]<<endl;
			uint64_t timing = featureName[*it][i][0].asLargestUInt();
			//cout<<"address: "<<address<<", timing: "<<timing<<endl; 
			(*(interdepTiming_))[address][timing] += featureName[*it][i][1].asUInt();

		}
	}
}


Json::Value* InterdepartTimeTestHandler::DataToJson() const
{
	Json::Value *vect = new Json::Value();

	for ( map<uint32_t ,map<uint64_t, uint32_t> >::iterator it=interdepTiming_->begin(); it!=interdepTiming_->end(); ++it){
		Json::Value addressVec(Json::arrayValue);
		for (map<uint64_t, uint32_t>::iterator itList=it->second.begin(); itList!=it->second.end(); ++itList){
			Json::Value timingVec(Json::arrayValue);
			timingVec.append((Json::LargestUInt)itList->first) ;
			timingVec.append( itList->second);
			addressVec.append(timingVec);
		}
		ostringstream convert;
		convert << it->first;
		(*vect)[convert.str()] = addressVec;
	}
	return vect;
}

int InterdepartTimeTestHandler::getTestResult(){
	struct in_addr host;
	for(std::list<std::tuple<uint64_t, double,double> >::iterator iter = dStats_.begin(); iter != dStats_.end(); iter++){
		host.s_addr = get<0>(*iter);
		cout<<"Host at "<<setw(14)<< inet_ntoa(host);
		if(get<1>(*iter) > get<2>(*iter) )
			cout<<" have "<<printInRed("abnormal")<<" interdeparture behavior.";
		else
			cout<<" have "<<printInGreen("normal") <<" interdeparture behavior.";
		cout<<" (dstat: "<<get<1>(*iter)<<", seuil:"<<get<2>(*iter)<<")"<<endl ;
	}
	return 0;
}
void InterdepartTimeTestHandler::runTest(){

	for(map<uint32_t, map<uint64_t, uint32_t> >::iterator networkIter = interdepTiming_->begin(); networkIter != interdepTiming_->end(); networkIter++){

		map<uint64_t, uint32_t>  modelAddressTiming = networkIter->second;
		
		//Compute the number of elements for a particular address in model
		uint32_t numOfElemModelAddrTiming = computeNumElement(&modelAddressTiming);
		//Compute the max interdepature timing of the distribution of that address
		uint64_t maxModelTiming = computeMaxTiming(&modelAddressTiming);
		//Compute cumulative distribution for a particular addresse in model 
		vector<tuple<uint64_t,uint64_t, double> > modelCumulDist;
		double newSum = 0;
		uint64_t lastTiming = 0;
		for(map<uint64_t, uint32_t>::iterator it = modelAddressTiming.begin(); it != modelAddressTiming.end(); it++ ){
			modelCumulDist.push_back(tuple<uint64_t,uint64_t, double>(lastTiming, it->first,  newSum ));
			newSum += (it->second/(double)numOfElemModelAddrTiming);
			lastTiming = it->first +1;
		}
		modelCumulDist.push_back(tuple<uint64_t,uint64_t, double>(maxModelTiming, maxModelTiming,  newSum ));


		map<uint64_t, uint32_t> testAddressTiming = (*testInterdepTiming_)[networkIter->first];
		//Compute the number of elements for a particular address in test
		uint32_t numOfElemTestAddrTiming = computeNumElement(&testAddressTiming);
		//Compute the max interdepature timing of the distribution of that address
		uint64_t maxTestTiming = computeMaxTiming(&testAddressTiming);
		//Compute cumulative distribution for a particular addresse in test 
		vector<tuple<uint64_t,uint64_t, double> > testCumulDist;
		newSum = 0;
		lastTiming = 0;
		for(map<uint64_t, uint32_t>::iterator it = testAddressTiming.begin(); it != testAddressTiming.end(); it++ ){
			testCumulDist.push_back(tuple<uint64_t,uint64_t, double>(lastTiming, it->first,  newSum ));
			newSum += (it->second/(double)numOfElemTestAddrTiming);
			lastTiming = it->first + 1;
		}

		
		testCumulDist.push_back(tuple<uint64_t,uint64_t, double>(maxTestTiming, maxTestTiming,  newSum ));

		double modelValue = 0;
		double testValue = 0;
		
		double dStat = 0;

		uint32_t maxModel =  modelCumulDist.size();
		uint32_t maxTest =  testCumulDist.size();
		uint32_t iterModel = 0, iterTest = 0;
		while(iterModel < maxModel || iterTest < maxTest){
			uint32_t value;
			if(iterModel < maxModel){
				value = get<0>(modelCumulDist[iterModel]);
				++iterModel;
			}else{
				value = get<0>(testCumulDist[iterTest]);
				++iterTest;
			}
			//cout<<"value: "<< value<<endl;
			if(value >= maxModelTiming)
				modelValue = 1;
			else
				modelValue = findValueInCumul(modelCumulDist, value);
			
			if(value >= maxTestTiming)
				testValue = 1;
			else
				testValue = findValueInCumul(testCumulDist, value);
			double tmp = modelValue - testValue;
			if (dStat < fabs(tmp)){
					dStat = fabs(tmp);
			}
		}

		double nModel = (double)numOfElemModelAddrTiming;
		double nTest = (double) numOfElemTestAddrTiming;
		if(nTest == 0)
			nTest = DBL_MAX;
		double c_alpah = 1.22;
		double seuil = c_alpah*pow((nModel + nTest)/(nModel * nTest), 0.5);

		//cout<<"Address: "<<networkIter->first<<" , dStat: "<< dStat <<endl;
		dStats_.push_back(make_tuple(networkIter->first, dStat, seuil));
	}
}

void InterdepartTimeTestHandler::printDistribution() const
{
	for ( map<uint32_t, map<uint64_t, uint32_t> >::iterator it=interdepTiming_->begin(); it!=interdepTiming_->end(); ++it){
		cout<<"\n\n"<<"adresse :"<< it->first <<" interdep: "<<endl;
		for (  map<uint64_t, uint32_t>::iterator itList=it->second.begin(); itList!=it->second.end(); ++itList){
			cout<<"\t"<<itList->first << " : "<<itList->second<<endl;
		}
	}
}

	
void InterdepartTimeTestHandler::ComputeInterdeparture(map<uint32_t, list<uint64_t> > *packetTiming, map<uint32_t, map<uint64_t, uint32_t> > * dist){
	for ( map<uint32_t, list<uint64_t> >::iterator it=packetTiming->begin(); it!=packetTiming->end(); ++it){
		if(it->second.size() > 1){
			list<uint64_t>::iterator itList = it->second.begin();
			++itList;
			
			for (  ; itList!=it->second.end(); ++itList){ 		//start at the second element
				uint64_t first = *(--itList); // go back one element
				uint64_t second = *(++itList); // return to the current one 
				
				(*dist)[it->first][second - first]++; 
			}
		}
	}
}

uint32_t InterdepartTimeTestHandler::computeNumElement(map<uint64_t, uint32_t> *dist)
{
	uint32_t size = 0;
	for ( map<uint64_t, uint32_t> ::iterator it=dist->begin(); it!=dist->end(); ++it){
		size += it->second;
	}
	return size;
}

uint64_t InterdepartTimeTestHandler::computeMaxTiming(map<uint64_t, uint32_t> *dist){
	uint64_t max = 0, i = 0;
	for ( map<uint64_t, uint32_t> ::reverse_iterator it=dist->rbegin(); it!=dist->rend(); ++it){
		if(it->first > max){
			max = it->first;
		}
		 i++;
	}
	return max;
}

double InterdepartTimeTestHandler::findValueInCumul(std::vector<tuple<uint64_t,uint64_t, double> > cumul, uint64_t value)
{
	uint32_t min = 0, max = cumul.size();
		
	while(max >= min){
		uint32_t mid = min + (max-min)/2;
		//	std::cout<<"\ts: "<<get<0>(cumul[0])<<", e: "<<get<1>(cumul[0])<< ", v: "<<get<2>(cumul[0])<<endl;
		//std::cout<<"\ts: "<<get<0>(cumul[1])<<", e: "<<get<1>(cumul[1])<< ", v: "<<get<2>(cumul[1])<<endl;
		//std::cout<<"\ts: "<<get<0>(cumul[2])<<", e: "<<get<1>(cumul[2])<< ", v: "<<get<2>(cumul[2])<<endl;


	//std::cout<<"s: "<<get<0>(cumul[mid])<<", e: "<<get<1>(cumul[mid])<< ", v: "<<get<2>(cumul[mid])<<endl;

		if(get<0>(cumul[mid]) > value)
			max = mid - 1;
		else if(get<1>(cumul[mid]) < value)
			min = mid  + 1;
		else //if(get<0>(cumul[mid]) <= value && get<1>(cumul[mid]) >= value)
			return get<2>(cumul[mid]);
	}
	assert(false); 
}
