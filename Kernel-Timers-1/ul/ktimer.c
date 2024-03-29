#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s [-s <sec> <msg>] | [-m <count>] | [-l]\n", argv[0]);
        return 1;
    }

    char buffer[256]; // Assuming a maximum length for the buffer

    // Clear the buffer
    memset(buffer, 0, sizeof(buffer));

    // Format the command line arguments
    if (strcmp(argv[1], "-s") == 0 && argc >= 4) {
        sprintf(buffer, "-s %s %s", argv[2], argv[3]);
        printf("-s %s %s\n", argv[2], argv[3]);

    } else if (strcmp(argv[1], "-m") == 0 && argc >= 3) {
        sprintf(buffer, "-m %s", argv[2]);
        printf("-m %s\n", argv[2]);

    } else if (strcmp(argv[1], "-l") == 0) {
        sprintf(buffer, "-l");
        printf("-l\n");
    } else {
        printf("Invalid arguments\n");
        return 1;
    }

    // Open the character device file for writing
    FILE *file = fopen("/dev/mytimer", "w");
    if (file == NULL) {
        perror("Failed to open character device file");
        return 1;
    }

    // Write the formatted data to the character device file
    if (fprintf(file, "%s\n", buffer) < 0) {
        perror("Error writing to character device file");
        fclose(file);
        return 1;
    }

    // Close the file
    fclose(file);
    return 0;
}


