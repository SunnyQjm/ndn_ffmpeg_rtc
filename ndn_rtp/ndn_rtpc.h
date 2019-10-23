#ifndef _NDN_RTPC_H_
#define _NDN_RTPC_H_

#include <stdint.h>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <ndn-cxx/face.hpp>
#include <errno.h>
#include <exception>
#include <sys/time.h>
#include <queue>

#include "recv_list.h"

using std::string ;
using std::queue ;
using namespace ndn ;

struct frame_t{
	frame_t():len(0),data(0){ }
	~frame_t(){ 
		if(data) free(data) ;
	}
	int len ;
	char *data ;
} ;

class ndn_rtpc
{
public:
	ndn_rtpc(const string &prefix);
	~ndn_rtpc();
	int readobj(char *buff) ;
private:
	void process_prei(const Interest& interest) ;
	void process_datai(const Interest& interest) ;
	void onInterest(const InterestFilter& filter, const Interest& interest) ;
	void onRegisterFailed(const Name& prefix, const std::string& reason) ;
	void onData(const Interest& interest , const Data& data);
	void onData_detect(const Interest& interest , const Data& data);
	void onNack(const Interest& interest, const lp::Nack& nack);
	void onNack_detect(const Interest& interest, const lp::Nack& nack);
	void onTimeout(const Interest& interest) ;
	void onTimeout_detect(const Interest& interest) ;

	void send_detect() ;
	void myExpressInterest(string name, long lifetime) ;
	void myExpressInterest_detect(string name, long lifetime) ;
	static void *run(void *param) ;

	void getFrame() ;

private:
	/* data */
	string prefix ;
	uint32_t pkt_seq ;
	string start_time ;

	pthread_mutex_t frame_mutex ;
	pthread_cond_t has_frame ;

	RecvList recvList ;
	queue<struct frame_t*> myFrameQ ;

	Face m_face ;
	KeyChain m_keyChain ;

};

#endif 
