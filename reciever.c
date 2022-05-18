#include "ether_types.h"



int main(int argc, char *argv[]) {

    unsigned char data_buffer[DATA_BUFFER_SIZE];
    unsigned char destination_mac_addr[6]; 
    char recieve_buffer[ETH_FRAME_LEN];

    struct sockaddr_ll temp_sockaddr_ll;
    memset(&temp_sockaddr_ll, 0x00, sizeof(struct sockaddr_ll));
    socklen_t sll_len = (socklen_t)sizeof(temp_sockaddr_ll);



    int ethernet_socket_fd = socket(AF_PACKET, SOCK_RAW, htons(proto));
    while(recvfrom(ethernet_socket_fd, recieve_buffer, ETH_FRAME_LEN, 0, (struct sockaddr *)&temp_sockaddr_ll, &sll_len) == 0 ) {

        continue;

    }

    memcpy(destination_mac_addr, &(temp_sockaddr_ll.sll_addr), sizeof(destination_mac_addr));
    ethernet_frame recive_data;
    memcpy(&recive_data, recieve_buffer, ETH_FRAME_LEN);
    data_message_by_fields temp_data_by_fields;
    memcpy(&(temp_data_by_fields.message_fields), &(recive_data.field.data), ETH_DATA_LEN);
    printf("%s\n", temp_data_by_fields.message_fields.command_message);

    //Get any properties from ether device//
    int interface_index    = get_interface_index(ethernet_socket_fd);
    char * own_mac_address = get_own_mac_address(ethernet_socket_fd);
    ////////////////////////////////////////
    
    ethernet_frame test_eth_frame_main;
    fill_eth_frame(&test_eth_frame_main, destination_mac_addr, own_mac_address, data_buffer, DATA_BUFFER_SIZE);

    data_message_by_fields recv_response;
    memcpy(&(recv_response.message_fields.command_message), CLIENT_HANDSHAKE_RESPONSE, 16);
    memcpy(&(test_eth_frame_main.field.data), &(recv_response.packed_data), ETH_DATA_LEN);



    if(sendto(ethernet_socket_fd, test_eth_frame_main.eth_buffer, ETH_FRAME_LEN, 0, (struct sockaddr*)&temp_sockaddr_ll, sizeof(temp_sockaddr_ll)) > 0) {
        printf("The response was been sent....\n");
    }

    recive_ordinary_file(ethernet_socket_fd, &temp_sockaddr_ll, &test_eth_frame_main);

}