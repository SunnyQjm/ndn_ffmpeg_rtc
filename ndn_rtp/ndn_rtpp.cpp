#include <iostream>
#include "ndn_rtpp.h"

using namespace std;

ndn_rtpp::ndn_rtpp(const string &prefix, const string& host, const int port)
        : pkt_seq(0)
{
    // 如果制定了IP和端口以TCP的方式连接一个路由器，则使用TCP Transport，否则使用默认的Transport
    if(host.empty()) {
        m_face = new Face();
    } else {
        m_face = new Face(host, to_string(port));
    }
    struct timeval start_t ;
    gettimeofday(&start_t,NULL) ;
    this->start_time = to_string(start_t.tv_sec) ;
    if(prefix[prefix.size()-1] == '/'){
        this->prefix = prefix.substr(0,prefix.size()-1) ;
    }else{
        this->prefix = prefix ;
    }
    start() ;
}

ndn_rtpp::~ndn_rtpp(){
    delete m_face;
}

int ndn_rtpp::sendobj(const char *data, int len){
    int pkt_n = len/8000 ;
    if(len%8000 != 0) pkt_n ++ ;
    // name : [prefix] / [start_time] / pkt_seq
    string basename = prefix + "/" + start_time +"/" ;
    char buff[8400] ;
    for (int i = 0; i < pkt_n; i++) {
        Data data_pkt ;
        data_pkt.setName(basename + to_string(pkt_seq++)) ;
        int content_len = 8000 ;
        if(len - i*8000 < 8000) content_len = len - i*8000 ;
        if(i == 0){
            *((int*)buff) = len ;
        }else{
            *((int*)buff) = -i ;
        }
        // data : [len|father]+[content]
        memcpy(buff+sizeof(int), data+i*8000, content_len) ;
        data_pkt.setContent(reinterpret_cast<const uint8_t*>(buff),
                            content_len+4) ;
        this->m_keyChain.sign(data_pkt) ;
        this->m_face->put(data_pkt) ;
//        cout << "put data : " << data_pkt.getName() << endl ;
    }
}

void ndn_rtpp::start(){
    pthread_t tid ;
    pthread_create(&tid,NULL,run,(void*)this) ;
}

void *ndn_rtpp::run(void *param){
    ndn_rtpp *_this = (ndn_rtpp*)param ;
    Face *face = _this->m_face ;
    face->setInterestFilter(_this->prefix ,
                           bind(&ndn_rtpp::onInterest, _this, _1, _2) ,
                           [](const Name& name) {
        cout << "register success: " << name.toUri() << endl;
    },
                           bind(&ndn_rtpp::onRegisterFailed, _this, _1, _2)) ;
    face->processEvents(time::milliseconds::zero(), true) ;
}

void ndn_rtpp::onInterest(const InterestFilter& filter, const Interest& interest) {
//    cout << "onInterest : " << interest.getName() << endl ;
    if(!interest.getMustBeFresh()){
//        cout << "data interest" << endl ;
        return ;
    }
    Data sig_pkt ;
    sig_pkt.setName(interest.getName()) ;
    char buff[80] ;
    *((int*)buff) = 0 ;
    *((uint32_t*)(buff+4)) = pkt_seq ;
    memcpy(buff+8, start_time.data(),start_time.size()) ;
    sig_pkt.setContent(reinterpret_cast<const uint8_t*>(buff),
                       8+start_time.size()) ;
    sig_pkt.setFreshnessPeriod(0_s) ;
    this->m_keyChain.sign(sig_pkt) ;
    this->m_face->put(sig_pkt) ;
}

void ndn_rtpp::onRegisterFailed(const Name& prefix, const std::string& reason) {
    cout << "onRegisterFailed: " << reason << endl;
}
