#include "ether_types.h"


int main(int argc, char *argv[]) {

    unsigned char data_buffer[DATA_BUFFER_SIZE];
    unsigned short proto = 0x1234;
    const unsigned char destination_mac_addr[] = { 0x00, 0x15, 0x5d, 0x4d, 0x63, 0xb0 }; 

    int file_size = get_full_path_file_size(argv[1]);
    if(file_size == -1) {

        printf("Bad file path!\n");

    }

    printf("%d\n", file_size);

    gets(data_buffer);

    int ethernet_socket_fd = socket(AF_PACKET, SOCK_RAW, htons(proto));
    if(ethernet_socket_fd < 0) {

        perror("ERROR: <socket()>");
        exit(EXIT_FAILURE);

    }

    //Get any properties from ether device//
    int interface_index    = get_interface_index(ethernet_socket_fd);
    char * own_mac_address = get_own_mac_address(ethernet_socket_fd);
    ////////////////////////////////////////

    ethernet_frame test_eth_frame;
    fill_eth_frame(&test_eth_frame, destination_mac_addr, own_mac_address, data_buffer, DATA_BUFFER_SIZE);

    struct sockaddr_ll temp_sockaddr_ll;
    fill_sockaddr_ll(&temp_sockaddr_ll, interface_index, destination_mac_addr);


    if(sendto(ethernet_socket_fd, test_eth_frame.eth_buffer, 1024, 0, (struct sockaddr*)&temp_sockaddr_ll, sizeof(temp_sockaddr_ll)) > 0) {

        printf("Good sending!\n");

    }
        

    close(ethernet_socket_fd);
}