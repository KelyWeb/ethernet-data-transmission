#include "ether_types.h"


int get_interface_index(const int socket_index) {

    int result_index = 0;
    struct ifreq buffer_for_get_index; //Struct which stores all data about network interface

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
    (filling_frame -> field).ethernet_header.h_proto = htons(proto);
    memcpy((filling_frame -> field).data, data_buffer, data_buffer_size);

}


void fill_sockaddr_ll(struct sockaddr_ll * filling_struct, const int interface_id, const char * dest_mac) {

    memset(filling_struct, 0, sizeof(struct sockaddr_ll));

    filling_struct -> sll_family  = PF_PACKET;
    filling_struct -> sll_ifindex = interface_id; 
    filling_struct -> sll_halen   = ETH_ALEN;

    memcpy((void *)(filling_struct -> sll_addr), (void *)dest_mac, ETH_ALEN);
    
}


int convert_char_mac_address(char * str_mac_address, unsigned char * bytes_mac_address) {

    int bytes_convert_size = sscanf(str_mac_address, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", 
                                                               &bytes_mac_address[0],
                                                               &bytes_mac_address[1],
                                                               &bytes_mac_address[2],
                                                               &bytes_mac_address[3],
                                                               &bytes_mac_address[4],
                                                               &bytes_mac_address[5]);                      
    return bytes_convert_size;

}


void forming_handshake_message(data_message_by_fields * message) {

    if(message == NULL) return;

    message -> message_fields.operation_type = 1;
    message -> message_fields.packet_number  = 0;
    strcpy(message -> message_fields.command_message, HANDSHAKE_MESSAGE);
    
}


int data_repacking_in_eth(union ethernet_frame * repack_frame, union data_message_by_fields * repack_message) {

    if(repack_frame == NULL || repack_message == NULL) return -1;

    memcpy(&(repack_frame -> field.data), repack_message -> packed_data, ETH_DATA_LEN);

    return 1;
}


int data_repacking_in_msg(union ethernet_frame * repack_frame, union data_message_by_fields * repack_message) {

    if(repack_frame == NULL || repack_message == NULL) return -1;

    memcpy(repack_message -> packed_data, &(repack_frame -> field.data), ETH_DATA_LEN);

    return 1;
}


int send_ordinary_file(const char * file_path, const int s_socket, const struct sockaddr_ll const * socket_struct, 
                        ethernet_frame * main_frame) {

    int file_dt = open(file_path, O_RDONLY);
    if(file_dt < 0) return -1;

    data_message_by_fields message_with_data;
    strcpy(message_with_data.message_fields.command_message, FILE_SEND_MESSAGE);
    data_repacking_in_eth(main_frame, &message_with_data);

    if(sendto(s_socket, main_frame -> eth_buffer, ETH_FRAME_LEN, 0, (struct sockaddr *)socket_struct, sizeof(struct sockaddr_ll)) > 0) {

        printf("-------------------Request to send a file------------------\n");

    }
    
    ethernet_frame temp_response_frame;
    struct sockaddr_ll temp_response_socket_struct;
    memset(&temp_response_socket_struct, 0x00, sizeof(temp_response_socket_struct));
    socklen_t sll_len = (socklen_t)sizeof(temp_response_socket_struct);
    
    int temp_check_recieve = 0;
    while((temp_check_recieve = recvfrom(s_socket, &(temp_response_frame.eth_buffer), ETH_FRAME_LEN, 0, NULL, &sll_len)) == 0) {

        continue;

    }
    data_repacking_in_msg(&temp_response_frame, &message_with_data);

    forming_frame_with_filename(file_path, &message_with_data);
    data_repacking_in_eth(main_frame, &message_with_data);
    if(sendto(s_socket, main_frame -> eth_buffer, ETH_FRAME_LEN, 0, (struct sockaddr *)socket_struct, sizeof(struct sockaddr_ll)) > 0) {

        printf("----------------------File name sent-----------------------\n");

    }

    char file_buffer[1449];
    memset(file_buffer, 0x00, sizeof(file_buffer));
    
    while(read(file_dt, (void *)file_buffer, 1449) > 0) {

        forming_frame_with_file_data(file_buffer, &message_with_data);
        data_repacking_in_eth(main_frame, &message_with_data);
        sendto(s_socket, main_frame -> eth_buffer, ETH_FRAME_LEN, 0, (struct sockaddr *)socket_struct, sizeof(struct sockaddr_ll));

        while((temp_check_recieve = recvfrom(s_socket, &(temp_response_frame.eth_buffer), ETH_FRAME_LEN, 0, NULL, &sll_len)) == 0) {

            continue;

        }
    }

    strcpy(message_with_data.message_fields.command_message, FILE_DATA_END);
    data_repacking_in_eth(main_frame, &message_with_data);
    sendto(s_socket, main_frame -> eth_buffer, ETH_FRAME_LEN, 0, (struct sockaddr *)socket_struct, sizeof(struct sockaddr_ll));
}

int recive_ordinary_file(const int s_socket, const struct sockaddr_ll const * socket_struct, 
                        ethernet_frame * main_frame) {
    int file_dt = 0;

    socklen_t sll_len = (socklen_t)sizeof(struct sockaddr_ll);
    ethernet_frame temp_response_frame;
    data_message_by_fields temp_message;

    while(recvfrom(s_socket, &(temp_response_frame.eth_buffer), ETH_FRAME_LEN, 0, NULL, &sll_len) == 0) continue;

    data_repacking_in_msg(&temp_response_frame, &temp_message);
    if(strcmp(temp_message.message_fields.command_message, FILE_SEND_MESSAGE) == 0) {

        strcpy(temp_message.message_fields.command_message, FILE_SEND_RESPONSE);
        data_repacking_in_eth(main_frame, &temp_message);

        sendto(s_socket, main_frame -> eth_buffer, ETH_FRAME_LEN, 0, (struct sockaddr *)socket_struct, sizeof(struct sockaddr_ll));
    }

    while(recvfrom(s_socket, &(temp_response_frame.eth_buffer), ETH_FRAME_LEN, 0, NULL, &sll_len) == 0) continue;
    data_repacking_in_msg(&temp_response_frame, &temp_message);
    if(strcmp(temp_message.message_fields.command_message, SEND_FILE_NAME) == 0) {

        file_dt = open(temp_message.message_fields.transmitted_data, O_RDWR | O_CREAT);

        while(1) {

            while(recvfrom(s_socket, &(temp_response_frame.eth_buffer), ETH_FRAME_LEN, 0, NULL, &sll_len) == 0) continue;
            
            data_repacking_in_msg(&temp_response_frame, &temp_message);

            if(strcmp(temp_message.message_fields.command_message, FILE_DATA_END) == 0) {

                close(file_dt);

                break;

            } else {

                write(file_dt, (void *)temp_message.message_fields.transmitted_data, 1449);
                strcpy(temp_message.message_fields.command_message, DATA_ACCEPTANCE);
                data_repacking_in_eth(main_frame, &temp_message);
                sendto(s_socket, main_frame -> eth_buffer, ETH_FRAME_LEN, 0, (struct sockaddr *)socket_struct, sizeof(struct sockaddr_ll));

            }
        }
    }

    return 1;
}


int forming_frame_with_filename(char * file_path, data_message_by_fields * main_frame) {

    char * cut_file_name = basename(file_path);
    if(cut_file_name == NULL) return -1;

    strcpy(main_frame -> message_fields.transmitted_data, cut_file_name);
    strcpy(main_frame -> message_fields.command_message, SEND_FILE_NAME);

    return 1;
}


int forming_frame_with_file_data(char * file_data, data_message_by_fields * main_frame) {

    if(file_data == NULL || main_frame == NULL) return -1;

    strcpy(main_frame -> message_fields.transmitted_data, file_data);
    strcpy(main_frame -> message_fields.command_message, FILE_DATA);

    return 1;
}