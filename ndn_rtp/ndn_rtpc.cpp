#include <iostream>
#include "ndn_rtpc.h"

using namespace std;

ndn_rtpc::ndn_rtpc(const string &prefix)
	:pkt_seq(-1),
	start_time("")
{
	this->prefix = prefix ;
	pthread_cond_init(&has_frame,NULL) ;
	pthread_mutex_init(&frame_mutex,NULL) ;
	pthread_t tid ;
	pthread_create(&tid,NULL,run,(void*)&m_face) ;
	send_detect() ;
}

ndn_rtpc::~ndn_rtpc(){

}

int ndn_rtpc::readobj(char *buff){
	pthread_mutex_lock(&frame_mutex) ;
	int objLen = 0 ;
	while(myFrameQ.empty()){
		pthread_cond_wait(&has_frame,&frame_mutex) ;
	}
	struct frame_t *frame = myFrameQ.front() ;
	myFrameQ.pop() ;
	pthread_mutex_unlock(&frame_mutex) ;
	memcpy(buff,frame->data,frame->len) ;
	objLen = frame->len ;
	delete(frame) ;
	return objLen ;
}

void ndn_rtpc::send_detect(){
	string name = prefix + "/manifest" ;
	myExpressInterest_detect(name, 500) ;
}

void *ndn_rtpc::run(void *param){
	Face *face_p = (Face*)param ;
	face_p->processEvents(time::milliseconds::zero(), true) ;
}


void ndn_rtpc::myExpressInterest(string name, long lifetime){
	Interest interest(Name(name.data())) ;
	boost::chrono::milliseconds lifetime_ms(lifetime) ;
	interest.setInterestLifetime(lifetime_ms) ;
	this->m_face.expressInterest(interest , 
			bind(&ndn_rtpc::onData,this,_1,_2),
			bind(&ndn_rtpc::onNack,this,_1,_2),
			bind(&ndn_rtpc::onTimeout,this,_1));
}

void ndn_rtpc::myExpressInterest_detect(string name , long lifetime){
	Interest interest(Name(name.data())) ;	
	boost::chrono::milliseconds lifetime_ms(lifetime) ;
	interest.setInterestLifetime(lifetime_ms) ;
	interest.setMustBeFresh(true) ;
	this->m_face.expressInterest(interest , 
			bind(&ndn_rtpc::onData_detect,this,_1,_2),
			bind(&ndn_rtpc::onNack_detect,this,_1,_2),
			bind(&ndn_rtpc::onTimeout_detect,this,_1));
	cout << "expressInterest : " << interest.getName() << endl ;
}

void ndn_rtpc::onData_detect(const Interest& interest , const Data& data){
	char buff[80] ;
	int content_size = data.getContent().value_size() ;
	cout<< "content_size = " << content_size << endl ;
	memcpy(buff, data.getContent().value(), content_size ) ;
	uint32_t seq = *((uint32_t*)(buff+4)) ;
	buff[content_size] = 0 ;
	this->start_time = buff+8 ;
	recvList.setFrontIndicate(seq) ;
	cout << "start_time = " << start_time << endl ;
	if(this->pkt_seq == -1) this->pkt_seq = seq ;
	while(this->pkt_seq < seq + 4){
		string name = prefix+"/"+start_time + "/"+ to_string(pkt_seq) ;
		myExpressInterest(name, 100) ;
		this->pkt_seq ++ ;
	}
}

void ndn_rtpc::onNack_detect(const Interest& interest, const lp::Nack& nack){
	cout << "Nack : no route to " << interest.getName() << endl ;
	onTimeout_detect(interest) ;
}

void ndn_rtpc::onTimeout_detect(const Interest& interest){
	long lifetime = interest.getInterestLifetime().count() ;
	if(lifetime > 1000){
		this->m_face.shutdown() ;
		exit(0) ;
		return ;
	}
	sleep(1) ;
	myExpressInterest_detect(interest.getName().toUri() , lifetime+200) ;
}

void ndn_rtpc::getFrame(){
	bool needSig = false ;
	while(recvList.hasFrame()){
		struct frame_t *frame = new frame_t;
		frame->len = recvList.getFrame(&(frame->data)) ;
		cout << "getFrame : len = " << frame->len << endl ;
		needSig = this->myFrameQ.empty() ;
		pthread_mutex_lock(&frame_mutex) ;
		this->myFrameQ.push(frame) ;
		pthread_mutex_unlock(&frame_mutex) ;
		if(needSig) pthread_cond_signal(&has_frame) ;
	}
}

void ndn_rtpc::onData(const Interest& interest , const Data& data){
	string newInterestname = prefix+"/"+start_time + "/"+ to_string(pkt_seq++) ;
	myExpressInterest(newInterestname, 100) ;

	string name = interest.getName().toUri() ;
	int idx1 = name.find_last_of('/') + 1 ;
	int cur_pkt_seq = atoi(name.substr(idx1,name.size() - idx1).data()) ;
	cout << "packet seq = " << cur_pkt_seq << endl ;
	char buff[10000] ;
	int len = data.getContent().value_size() ;
	memcpy(buff , data.getContent().value(), len) ;
	int frameLen = *((int*)buff);
	cout << "frameLen = " << frameLen << endl ;
	int nextN = frameLen / 8000 ;
	if(frameLen < 0) nextN = -1 ;
	else if(frameLen % 8000 != 0 ) nextN ++ ;
	recvList.insertPkt(buff+sizeof(int) , len-sizeof(int) , 
			nextN , cur_pkt_seq) ;
	getFrame() ;
}

void ndn_rtpc::onNack(const Interest& interest, const lp::Nack& nack){
	cout << "Nack : no route to " << interest.getName() << endl ;
}

void ndn_rtpc::onTimeout(const Interest& interest) {
	long lifetime = interest.getInterestLifetime().count() ;
	if(lifetime > 1000){
		this->m_face.shutdown() ;
		exit(0) ;
		return ;
	}
	myExpressInterest(interest.getName().toUri() , lifetime+200) ;
}
