project(ndn_rtp)
add_library(ndn_rtp SHARED ndn_rtpc.cpp ndn_rtpc.h ndn_rtpp.cpp ndn_rtpp.h recv_list.cpp recv_list.h) 
target_link_libraries(ndn_rtp pthread pcap ndn-cxx boost_system)

