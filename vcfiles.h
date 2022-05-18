#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

static const char * FILE_PATH_ERROR = "ERROR: BAD FILE PATH";
static const char * FILE_TYPE_ERROR = "ERROR: FILE IS UNSUPPORTED";
static const char * EXECUTE_ERROR   = "ERROR: use ./main <dest mac-address>";


int get_full_path_file_size(const char * file_path);

void show_file_info(const char * file_name, struct stat * file_info, int tabble_size);

int show_dir_info(const char * dir_name, struct stat * file_info, int tabble_start_size);

void formate_pathsize_output(const int path_size);

int get_file_type(const char * file_path);