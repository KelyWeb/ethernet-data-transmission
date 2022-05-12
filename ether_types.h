#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <net/ethernet.h>
#include <netinet/ether.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>


static const unsigned int DATA_BUFFER_SIZE    = 1024;
static const char * WIFI_INTERFACE_NAME       = "wlp1s0";
static const char * ETH_INTERFACE_NAME        = "eth0";


//Ethernet result frame for sending//
typedef union ethernet_frame {

    struct { //Ethernet header by fields

        struct ethhdr ethernet_header;

        unsigned char data[ETH_DATA_LEN];

    } field;

    unsigned char eth_buffer[ETH_FRAME_LEN]; //Ethernet frame by full char buffer

} ethernet_frame;


int get_interface_index(const int socket_index);


char * get_own_mac_address(const int socket_index);


void fill_eth_frame(ethernet_frame * filling_frame, const char * dest_mac, const char * sourse_mac, 
                    const char * data_buffer, const int data_buffer_size);


void fill_sockaddr_ll(struct sockaddr_ll * filling_struct, const int interface_id, const char * dest_mac);