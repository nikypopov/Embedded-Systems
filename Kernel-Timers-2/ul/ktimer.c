#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

void sighandler(int);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s [-s <sec> <msg>] | [-m <count>] | [-l]\n", argv[0]);
        return 1;
    }

    /* setup to read proc entry */
    FILE *file;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    /* fasync setup*/
    int pFile, oflags;
    char buffer[256]; // Assuming a maximum length for the buffer
    struct sigaction action, oa;
    int ii, count = 0;

    pFile = open("/dev/mytimer", O_RDWR);
    if (pFile < 0) {
        perror("Failed to open character device file");
        return -1;
    }

    // Clear the buffer
    memset(buffer, 0, sizeof(buffer));

    // Setup signal handler
    memset(&action, 0, sizeof(action));
    action.sa_handler = sighandler;
    action.sa_flags = SA_SIGINFO;
    sigemptyset(&action.sa_mask);
    sigaction(SIGIO, &action, NULL);
    fcntl(pFile, F_SETOWN, getpid());
    oflags = fcntl(pFile, F_GETFL);
    fcntl(pFile, F_SETFL, oflags | FASYNC);

    // Format the command line arguments
    if (strcmp(argv[1], "-s") == 0 && argc >= 4) {
        sprintf(buffer, "-s %s %s", argv[2], argv[3]);
        // printf("-s %s %s\n", argv[2], argv[3]);
        write(pFile, buffer, strlen(buffer));
        pause();
        printf("%s\n", argv[3]);

    } else if (strcmp(argv[1], "-l") == 0) {
        file = fopen("/proc/mytimer", "r");
        if (file == NULL) {
            perror("fopen");
            exit(EXIT_FAILURE);
            }
        
        // Read and print the contents of the file
        while ((read = getline(&line, &len, file)) != -1) {
            printf("%s", line);
        }

        // Clean up
        free(line);
        fclose(file);
    } else if (strcmp(argv[1], "-m") == 0) {
        printf("Error: multiple timers not supported.\n");
    } else {
        printf("Invalid arguments\n");
        return 1;
    }
    
    close(pFile);
    return 0;
}

void sighandler(int signo)
{
    // printf("awake\n");

}

