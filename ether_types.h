#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <net/ethernet.h>
#include <netinet/ether.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <libgen.h>
#include <unistd.h>


static const unsigned short proto             = 0x1234;
static const unsigned int DATA_BUFFER_SIZE    = 1024;
static const char * WIFI_INTERFACE_NAME       = "wlp1s0";
static const char * ETH_INTERFACE_NAME        = "eth0";
static const int    MAC_ADDRESS_BYTES_SIZE    = 6;
static const char * HANDSHAKE_MESSAGE         = "/ping client";
static const char * CLIENT_HANDSHAKE_RESPONSE = "/client response";
static const char * FILE_SEND_MESSAGE         = "/start send file";
static const char * FILE_SEND_RESPONSE        = "/file start succsess";
static const char * SEND_FILE_NAME            = "/file name";
static const char * FILE_DATA                 = "/file data";
static const char * FILE_DATA_END             = "/file data end";
static const char * DATA_ACCEPTANCE           = "/data acceptence";

//Ethernet result frame for sending//
typedef union ethernet_frame {

    struct { //Ethernet header by fields

        struct ethhdr ethernet_header;

        unsigned char data[ETH_DATA_LEN];

    } field;

    unsigned char eth_buffer[ETH_FRAME_LEN]; //Ethernet frame by full char buffer

} ethernet_frame;


typedef union data_message_by_fields {

    struct {

        unsigned int  operation_type;
        unsigned int  packet_number;
        unsigned char command_message[42];
        unsigned char transmitted_data[1450];

    } message_fields;

    unsigned char packed_data[ETH_DATA_LEN];

} data_message_by_fields;


int get_interface_index(const int socket_index);


char * get_own_mac_address(const int socket_index);


void fill_eth_frame(ethernet_frame * filling_frame, const char * dest_mac, const char * sourse_mac, 
                    const char * data_buffer, const int data_buffer_size);


void fill_sockaddr_ll(struct sockaddr_ll * filling_struct, const int interface_id, const char * dest_mac);


int convert_char_mac_address(char * str_mac_address, unsigned char * bytes_mac_address);


void forming_handshake_message(data_message_by_fields * message);


int data_repacking_in_eth(union ethernet_frame * repack_frame, union data_message_by_fields * repack_message);


int data_repacking_in_msg(union ethernet_frame * repack_frame, union data_message_by_fields * repack_message);


int send_ordinary_file(const char * file_path, const int s_socket, const struct sockaddr_ll const * socket_struct, 
                        ethernet_frame * main_frame);


int recive_ordinary_file(const int s_socket, const struct sockaddr_ll const * socket_struct, 
                        ethernet_frame * main_frame);


int forming_frame_with_filename(char * file_path, data_message_by_fields * main_frame);


int forming_frame_with_file_data(char * file_data, data_message_by_fields * main_frame);