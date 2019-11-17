#ifndef _RECV_LIST_H_
#define _RECV_LIST_H_

enum {RECV_LIST_SZ = 1000} ;

struct pkt_slice_t{
	int nextN ;
	int len ;
	char *data ;
} ;

class RecvList
{
public:
	RecvList();
	~RecvList();

	void setFrontIndicate(uint32_t indicate) ;
	void insertPkt(char *pkt, int len ,int nextN, uint32_t seq) ;
	bool hasFrame() ;
	int getFrame(char **frame) ;
	void lossFrame(int seq) ;
	void lossFrame() ;

private:
	/* data */
	struct pkt_slice_t list[RECV_LIST_SZ] ;
	int front ;
	int back ;
	uint32_t front_indicate ;	//最左边代表的pkt_seq
};

#endif 
