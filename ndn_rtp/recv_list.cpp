#include <iostream>
#include <cstring>
#include "recv_list.h"

using namespace std;

RecvList::RecvList():
	front(0),
	back(0),
	front_indicate(0)
{
	memset(list,0,sizeof(list)) ;
}

RecvList::~RecvList(){

}

void RecvList::setFrontIndicate(uint32_t indicate){
	this->front_indicate = indicate ;
}

void RecvList::insertPkt(char *pkt, int len , int nextN,uint32_t seq){
	int insertIdx = (seq - front_indicate + front ) % RECV_LIST_SZ ;
	list[insertIdx].len =  len ;
	list[insertIdx].nextN = nextN ;
	list[insertIdx].data = (char*)malloc(len) ;
	memcpy(list[insertIdx].data , pkt , len) ;
	if(seq > back ) back = seq ;
}

bool RecvList::hasFrame(){
	while(list[front].nextN < 0){
		if(list[front].data) free(list[front].data) ;
		memset(&list[front],0,sizeof(struct pkt_slice_t)) ;
		front = (front+1) % RECV_LIST_SZ ;
		this->front_indicate ++ ;
	}
	int sliceN = list[front].nextN ;
	if(sliceN <= 0 ) return false ;
	for (int i = 0; i < sliceN; i++) {
		if(list[(front+i)%RECV_LIST_SZ].len <= 0) return false ;
	}
	return true ;
}

int RecvList::getFrame(char** frame){
	int sliceN = list[front].nextN ;
	*frame = (char*)malloc(sliceN*8000) ;
	int offset = 0 ;
	for (int i = 0; i < sliceN; i++) {
		memcpy((*frame)+offset, list[front].data, list[front].len) ;
		offset += list[front].len ;
		free(list[front].data) ;
		memset(&list[front],0,sizeof(struct pkt_slice_t)) ;
		front = (front+1) % RECV_LIST_SZ ;
		this->front_indicate ++ ;
	}
	//cout << "recvList : getFrame offset = " << offset << endl ;
	return offset ;
}
void RecvList::lossFrame(int seq) {
	cout << "lossFrame : seq = " << seq << endl ;
	int insertIdx = (seq - front_indicate + front ) % RECV_LIST_SZ ;
	list[insertIdx].nextN = -100 ;
	front_indicate += seq+1 ;
	int fronttmp = front ;
	front = (insertIdx + 1) % RECV_LIST_SZ ; ;
	while(fronttmp != front){
		if(list[fronttmp].data) free(list[fronttmp].data) ;
		memset(&list[fronttmp],0,sizeof(struct pkt_slice_t)) ;
		fronttmp = (fronttmp+1) % RECV_LIST_SZ ;
	}
}

void RecvList::lossFrame(){
	cout << "lossFrame : seq = " << endl ;
	while(front <= back){
		if(list[front].data) free(list[front].data) ;
		memset(&list[front],0,sizeof(struct pkt_slice_t)) ;
		front = (front+1) % RECV_LIST_SZ ;
	}
	front = 0 ;
	back = 0 ;
	front_indicate = 0 ;
}
