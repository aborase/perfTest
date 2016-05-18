#include "utils.h"

/*
 * This script measures write latency for small files.
 *
 * Script does the following
 * - Create a new directory for each file size.
 * - Create files in directory - either 25 MB of data or 5000 files.
 * - Run file_read test sequentially/in random access.
 * - The results reported are also written to file pointed to by file_rd.
 */
int main(int argc, char *argv[]) {

    if (argc != 4) {
        printf("Usage: ./latency_write <small/big> <mnt_directory> <is_sequential?>\n");
        printf("where small/big indicates what files to run test on.\n");
        printf("      mnt_directory is where the file system is mounted\n");
        printf("      is_sequential is set to 1 for sequential access and 0 for random acccess\n");
        exit(1);
    }

    int small_files[14]   = {0, 64, 128, 256, 512, 1024, 2048, 4096,
                             6144, 8192, 10240, 12288, 14336, 16384};
    int small_count = 14;

    int big_files[19] = {0, 32768, 65536, 131072, 262144,
                         524288, 1048576, 2097152, 4194304, 8388608, 16777216,
                         25165824, 33554432, 67108864, 100663296, 134217728,
                         268435456, 402653184, 536870912};
    int big_count = 19;

    int count = 0;

    int block_size = 0;
    int *array;
    if(strcmp(argv[1], "small") == 0) {
        array = small_files;
        block_size = 64;
        count = small_count;
    } else {
        array = big_files;
        block_size = 32 * 1024;
        count = big_count;
    }

    int cache = 400 * 1024 * 1024;

    char dir[19][6] = {"", "dir1", "dir2", "dir3", "dir4", "dir5", "dir6",
                       "dir7", "dir8", "dir9", "dir10", "dir11", "dir12",
                       "dir13", "dir14", "dir15", "dir16", "dir17", "dir18"};

    int i = 1;
    for (i = 1; i < count; i++) {
        // Make the directory.
        char my_path[100] =  {'\0'};
        strcat(my_path, argv[2]);
        strcat(my_path, "/");
        strcat(my_path, dir[i]);


        // Create the directory.
        char cmd[100] =  {'\0'};
        strcat(cmd, "mkdir ");
        strcat(cmd, my_path);

        printf("%s\n", cmd);
        system(cmd);

        // Find number of files to create. Cap at 5000.
        int file_count = cache / array[i] + 1;
        if (file_count > 1000) {
            file_count = 1000;
        }

        //prepare next command to run
        cmd[0] = '\0';
        strcat(cmd, "./file_write ");
        strcat(cmd, my_path);
        strcat(cmd, " ");
        strcat(cmd, argv[3]);
        strcat(cmd, " ");
        sprintf(cmd + strlen(cmd), "%d", block_size);
        strcat(cmd, " ");
        sprintf(cmd + strlen(cmd), "%d", file_count);
        strcat(cmd, " ");
        sprintf(cmd + strlen(cmd), "%d", array[i]);

        printf("%s\n", cmd);

        int j = 0;
        for (j = 0; j < 5; j++) {
            system(cmd);
        }

        int out = open("results.txt", O_CREAT | O_RDWR);
        if (lseek(out, 0, SEEK_END) == -1) {
            int err = errno;
            printf("Seek to end of file failed with errno %d\n", err);
            exit(1);
        }

        write(out, "\n", strlen("\n"));
    }

}
