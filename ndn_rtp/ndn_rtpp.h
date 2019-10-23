#ifndef _NDN_RTPP_H_
#define _NDN_RTPP_H_

#include <stdint.h>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <ndn-cxx/face.hpp>
#include <errno.h>
#include <exception>
#include <sys/time.h>

using std::string ;
using namespace ndn ;

class ndn_rtpp
{
public:
	ndn_rtpp(const string &prefix);
	~ndn_rtpp();
	void start() ;
	int sendobj(const char *data, int len) ;
private:
	void onInterest(const InterestFilter& filter, const Interest& interest) ;
	void onRegisterFailed(const Name& prefix, const std::string& reason) ;
	static void *run(void *param) ;
private:
	// ; /prefix/timestamp/pkt_seq
	uint32_t pkt_seq ;
	string prefix ;
	string start_time ;

	Face m_face ;
	KeyChain m_keyChain ;
};
#endif 
