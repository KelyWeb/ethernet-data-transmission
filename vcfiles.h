#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

static const char * FILE_TYPE_ERROR = "FILE HAS UNSUPPORTED";


int get_full_path_file_size(const char * file_path);

void show_file_info(const char * file_name, struct stat * file_info, int tabble_size);

int show_dir_info(const char * dir_name, struct stat * file_info, int tabble_start_size);