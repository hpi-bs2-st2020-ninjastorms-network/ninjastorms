#include <sys/types.h>
#include "kernel/network/ethernet_frame.h"

// takes raw input from driver and starts encapsulation
void start_pdu_encapsulation(uint8_t* buf);

// handles arp requests based on an ethernet frame
void handle_arp(ethernet_frame_t frame);