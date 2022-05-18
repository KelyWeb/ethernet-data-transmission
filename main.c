#include "ether_types.h"


int main(int argc, char *argv[]) {

    unsigned char data_buffer[DATA_BUFFER_SIZE];
    unsigned char destination_mac_addr[ETH_ALEN]; 
    
    
    if(argc != 3) {

        printf("Command for using utility: ./main <file/dir path> <dest mac-address>\n");
        exit(EXIT_FAILURE);

    }
    
    int convert_result = convert_char_mac_address(argv[2], destination_mac_addr);
    if(convert_result != MAC_ADDRESS_BYTES_SIZE) {

        printf("Invalid format of the mac address, try again.\n");
        exit(EXIT_FAILURE);

    } //Checking for the correctness of the dest char mac


    int file_size = get_full_path_file_size(argv[1]);
    if(file_size == -1) {

        printf("Incorect file path, try again.\n");

    } //Checking for the correctness of the file

    int ethernet_socket_fd = socket(AF_PACKET, SOCK_RAW, htons(proto));
    if(ethernet_socket_fd < 0) {

        perror("ERROR: <socket()>");
        exit(EXIT_FAILURE);

    }

    //Get any properties from ether device//
    int interface_index    = get_interface_index(ethernet_socket_fd);
    char * own_mac_address = get_own_mac_address(ethernet_socket_fd);
    ////////////////////////////////////////
    
    //Filling data about dest and src host//
    ethernet_frame test_eth_frame;
    fill_eth_frame(&test_eth_frame, destination_mac_addr, own_mac_address, data_buffer, DATA_BUFFER_SIZE);
    struct sockaddr_ll temp_sockaddr_ll;
    fill_sockaddr_ll(&temp_sockaddr_ll, interface_index, destination_mac_addr);
    ////////////////////////////////////////


    data_message_by_fields temp_message;
    forming_handshake_message(&temp_message);
    memcpy(&(test_eth_frame.field.data), &temp_message, ETH_DATA_LEN);
    if(sendto(ethernet_socket_fd, test_eth_frame.eth_buffer, ETH_FRAME_LEN, 0, (struct sockaddr*)&temp_sockaddr_ll, sizeof(temp_sockaddr_ll)) > 0) {

        printf("\n---------------Ping frame for dest-host sent---------------\n");

        printf("---------------------Wait for response---------------------\n");
    }


    ethernet_frame response_frame;
    struct sockaddr_ll temp_sockaddr_ll_for_recive;
    memset(&temp_sockaddr_ll_for_recive, 0x00, sizeof(temp_sockaddr_ll_for_recive));
    socklen_t sll_len = (socklen_t)sizeof(temp_sockaddr_ll_for_recive);

    int temp_check_recieve = 0;
    while((temp_check_recieve = recvfrom(ethernet_socket_fd, &(response_frame.eth_buffer), ETH_FRAME_LEN, 0, NULL, &sll_len)) == 0) {

        continue;

    }
    data_message_by_fields recive_message;
    memcpy(&(recive_message.packed_data), &(response_frame.field.data), ETH_DATA_LEN);
    if(strcmp(recive_message.message_fields.command_message, CLIENT_HANDSHAKE_RESPONSE) == 0) {

        sleep(3);
        printf("---------The Response from the client was received---------\n");

    }


    formate_pathsize_output(file_size);

    int client_choice = getchar();

    if(client_choice == 'y' || client_choice == 'Y') {

        int file_type_for_send = get_file_type(argv[1]);

        if(file_type_for_send == 1) {



        }
        if(file_type_for_send == 2) {

            send_ordinary_file(argv[1], ethernet_socket_fd, &temp_sockaddr_ll, &test_eth_frame);

        }

    }

    close(ethernet_socket_fd);
}