#include "vcfiles.h"


int get_full_path_file_size(const char * file_path) {

    struct stat file_full_info;

    if(stat(file_path, &file_full_info) == -1) return -1;
    else {

        if( S_ISREG(file_full_info.st_mode) ) { 

            show_file_info(file_path, &file_full_info, 0);
            return file_full_info.st_size;

        }
        else if( S_ISDIR(file_full_info.st_mode) ) {

            int result_dir_byte_size = show_dir_info(file_path, &file_full_info, 0);
            return result_dir_byte_size;

        }
        else {

            fputs(FILE_TYPE_ERROR, stdout);
            return -1;

        }
    }
}


void show_file_info(const char * file_name, struct stat * file_info, int tabble_size) {

    for(int i = 0; i < tabble_size; i++) printf(" ");
    if( S_ISDIR(file_info -> st_mode) ) {

        printf("%s\t| %d BYTES\t| DIR\n", file_name, file_info -> st_size);

    } else {

        printf("%s\t| %d BYTES\t| FILE\n", file_name, file_info -> st_size);

    }
}


int show_dir_info(const char * dir_name, struct stat * file_info, int tabble_start_size) {
    static int full_bytes_size   = 0;
    
    DIR * actuall_dir = opendir(dir_name);

    if(actuall_dir == NULL) return -1;
    struct dirent * s_file = NULL;

    full_bytes_size += file_info -> st_size;

    while( (s_file = readdir(actuall_dir)) != NULL) {

        if(!strcmp(s_file -> d_name, "..") || !strcmp(s_file -> d_name, ".")) continue;

        if(s_file -> d_type == DT_DIR) {

            char * update_dir_path = malloc(strlen(dir_name) + strlen(s_file -> d_name) + 5);

            strcpy(update_dir_path, dir_name);
            strcat(update_dir_path, s_file -> d_name);
            strcat(update_dir_path, "/");
            
            //stat(update_dir_path, file_info);
            show_file_info(dir_name, file_info, tabble_start_size);
            show_dir_info(update_dir_path, file_info, tabble_start_size + 4);
            
            free(update_dir_path);
        }
        if(s_file -> d_type == DT_REG) {

            char * update_dir_path = malloc(strlen(dir_name) + strlen(s_file -> d_name) + 5);
            strcpy(update_dir_path, dir_name);
            strcat(update_dir_path, s_file -> d_name);            
            
            struct stat temp_file_stat;
            stat(update_dir_path, &temp_file_stat);
            show_file_info(update_dir_path, &temp_file_stat, tabble_start_size + 6);

            full_bytes_size += temp_file_stat.st_size;
            free(update_dir_path);

        }
    }
    return full_bytes_size;
}


void formate_pathsize_output(const int path_size) {

    printf("FULL FILE SIZE: %d BYTES\n", path_size);
    printf("Do you really want to send this data? Y/N\n");
    
}


int get_file_type(const char * file_path) {

    if(file_path == NULL) return -1;

    struct stat file_info;
    stat(file_path, &file_info);

    if( S_ISDIR(file_info.st_mode) ) return 1;
    if( S_ISREG(file_info.st_mode) ) return 2;

    return -1;
}