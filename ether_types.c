#include "ether_types.h"


int get_interface_index(const int socket_index) {

    int result_index = 0;
    struct ifreq buffer_for_get_index;

    memset (&buffer_for_get_index, 0x00, sizeof(buffer_for_get_index));
    strncpy(buffer_for_get_index.ifr_name, WIFI_INTERFACE_NAME, IFNAMSIZ);
    ioctl  (socket_index, SIOCGIFINDEX, &buffer_for_get_index);

    result_index = buffer_for_get_index.ifr_ifindex;
    return result_index;

}


char * get_own_mac_address(const int socket_index) {

    struct ifreq buffer_for_get_mac_address;

    memset(&buffer_for_get_mac_address, 0x00, sizeof(buffer_for_get_mac_address));

    strncpy(buffer_for_get_mac_address.ifr_name, WIFI_INTERFACE_NAME, IFNAMSIZ);

    char * result_mac_address = malloc(sizeof(char) * ETH_ALEN);
    if(result_mac_address == NULL) return NULL;

    ioctl(socket_index, SIOCGIFHWADDR, &buffer_for_get_mac_address);

    memcpy((void *)result_mac_address, (void *)(buffer_for_get_mac_address.ifr_hwaddr.sa_data), ETH_ALEN);

    return result_mac_address;
}


void fill_eth_frame(ethernet_frame * filling_frame, const char * dest_mac, 
                    const char * sourse_mac, const char * data_buffer, const int data_buffer_size) {

    memcpy((filling_frame -> field).ethernet_header.h_dest,   dest_mac,   ETH_ALEN);
    memcpy((filling_frame -> field).ethernet_header.h_source, sourse_mac, ETH_ALEN);
    (filling_frame -> field).ethernet_header.h_proto = htons(0x1234);
    memcpy((filling_frame -> field).data, data_buffer, data_buffer_size);

}


void fill_sockaddr_ll(struct sockaddr_ll * filling_struct, const int interface_id, const char * dest_mac) {

    memset(filling_struct, 0, sizeof(struct sockaddr_ll));

    filling_struct -> sll_family  = PF_PACKET;
    filling_struct -> sll_ifindex = interface_id; 
    filling_struct -> sll_halen   = ETH_ALEN;

    memcpy((void *)(filling_struct -> sll_addr), (void *)dest_mac, ETH_ALEN);
    
}