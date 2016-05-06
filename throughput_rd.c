#include <omp.h>
#include "utils.h"

#define FLAGS                       (O_RDONLY | O_SYNC  | O_DIRECT) 
#define MAX_FILES                   1000
#define FILE_COUNT                  200

struct share_it {
    char*       buf;
    char*       filename;
    size_t      size;
    int         count;
    timestamp   duration;
    size_t*     total_bytes;  
};

int dummy_call(char* buf) {
    return (buf[0] == '0');
}

bool do_sequential(struct share_it* my_state) {
    size_t size         = my_state->size;
    timestamp start     = 0;
    timestamp end       = 0;
    int bytes           = 0;

    int i;
    for (i = 0; i < my_state->count; i++) { 
        // Pick a file in random and read that file
        // Pick a random file.
        int idx = rand() % MAX_FILES + 1;
        char num[5];
        sprintf(num, "%d", idx);

        char my_file[100] = {'\0'};
        strcat(my_file, my_state->filename);
        strcat(my_file, num);

        int fd = open(my_file, FLAGS);
        int err = errno;
        if (fd == -1) {
                printf("Could not open file descriptor for file %s. Error = %d\n",
                        my_file, err);
                exit(1);
        }

        RDTSCP(start);
        bytes = read(fd, my_state->buf, size);
        RDTSCP(end);
        my_state->duration  += (end - start);
        if (bytes <= 0 || bytes != size)
            return false;
        *(my_state->total_bytes) += bytes;

        close(fd);
    }

    return true;
}

int main(int argc, char **argv) {
    if ( argc != 4) {
        printf("Usage: ./throughput_rd <mnt_directory> <block_size> <num_of_threads>\n");
        exit(1);
    }

    // Get the mount directory
    char *path = argv[1];
    char *filename = "file";

    int block_size = atoi(argv[2]);

    // Set the number of threads.
    int nthreads = atoi(argv[3]);
    omp_set_num_threads(nthreads);

    int tid;

    double throughput = 0;

/* Fork a team of threads with each thread having a private tid variable */
#pragma omp parallel private(tid)
    {
        tid = omp_get_thread_num();

        // Setup
        // Prepare the file name
        char my_file[100] = {'\0'};
        strcat(my_file, path);
        strcat(my_file, "/");
        strcat(my_file, filename);

        time_t t;
        srand((unsigned) time(&t));
 
        char *buf = (char *)malloc(block_size);

        size_t total_bytes = 0;

        // Prepare for read.
        struct share_it state;
        state.filename    = my_file;
        state.buf         = buf;
        state.size        = block_size;
        state.count       = FILE_COUNT;
        state.duration    = 0;
        state.total_bytes = &total_bytes;

        // Wait to read
#pragma omp barrier

        int i = 0;
        int j = 0;
        for (i = 0; i < LOOP_COUNTER; i++) {
            bool success = do_sequential(&state);
            if (!success) {
                printf("%d : Read failed\n", tid);
                exit(1);
             }
        }

        //printf("Total bytes read is %ld\n", total_bytes);
        //printf("Total bytes read should be %ld\n", block_size * FILE_COUNT * LOOP_COUNTER);

        // Find the throughput of this thread
        double total_read = (double)total_bytes / (1024 * 1024 * 1024); // Gb
        double time_taken = (double)state.duration / (CPU_FREQ * 1000000); // seconds

        //printf("Total bytes read is %lf Gb\n", total_read);
        //printf("Time taken is %lf seconds\n", time_taken);

        double my_throughput = total_read / time_taken; // in GB/s


#pragma omp atomic
        throughput += my_throughput;

        free(buf);

    }  /* All threads join master thread and terminate */


    printf("%lf\n", (throughput)/nthreads);

}



