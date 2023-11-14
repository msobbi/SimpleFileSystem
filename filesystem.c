#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DISK_FILE "virtual_disk.bin"
#define DISK_SIZE 1024 * 1024  // 1 MB virtual disk
#define MAX_FILES 100  // Maximum number of files
#define FILE_SIZE 1024  // Maximum size of a file in bytes

typedef struct {
    char filename[20];
    int start;
    int size;
} FileEntry;

FileEntry fat[MAX_FILES];  // File Allocation Table

// Initialize a virtual disk
void init_disk() {
    FILE *disk = fopen(DISK_FILE, "wb");
    if (disk == NULL) {
        perror("Error creating virtual disk");
        exit(EXIT_FAILURE);
    }

    // Allocate disk space
    char *buffer = calloc(DISK_SIZE, sizeof(char));
    if (buffer == NULL) {
        perror("Memory allocation error");
        fclose(disk);
        exit(EXIT_FAILURE);
    }

    fwrite(buffer, sizeof(char), DISK_SIZE, disk);
    free(buffer);
    fclose(disk);

    // Initialize the FAT
    memset(fat, 0, sizeof(fat));
}

// Create a new file
int create_file(const char *filename, int size) {
    if (size > FILE_SIZE) {
        printf("Error: File size too large.\n");
        return -1;
    }

    // Find an empty slot in FAT
    for (int i = 0; i < MAX_FILES; i++) {
        if (fat[i].size == 0) {
            strncpy(fat[i].filename, filename, sizeof(fat[i].filename) - 1);
            fat[i].start = i * FILE_SIZE;
            fat[i].size = size;
            return i;
        }
    }

    printf("Error: No free slots available.\n");
    return -1;
}

// Write to a file
void write_file(int file_index, const char *data) {
    if (file_index < 0 || file_index >= MAX_FILES) {
        printf("Error: Invalid file index.\n");
        return;
    }

    if (fat[file_index].size == 0) {
        printf("Error: File does not exist.\n");
        return;
    }

    FILE *disk = fopen(DISK_FILE, "rb+");
    if (disk == NULL) {
        perror("Error opening virtual disk");
        return;
    }

    fseek(disk, fat[file_index].start, SEEK_SET);
    fwrite(data, sizeof(char), fat[file_index].size, disk);
    fclose(disk);
}

// Read a file
void read_file(int file_index, char *buffer) {
    if (file_index < 0 || file_index >= MAX_FILES) {
        printf("Error: Invalid file index.\n");
        return;
    }

    if (fat[file_index].size == 0) {
        printf("Error: File does not exist.\n");
        return;
    }

    FILE *disk = fopen(DISK_FILE, "rb");
    if (disk == NULL) {
        perror("Error opening virtual disk");
        return;
    }

    fseek(disk, fat[file_index].start, SEEK_SET);
    fread(buffer, sizeof(char), fat[file_index].size, disk);
    fclose(disk);
}

// Delete a file
void delete_file(int file_index) {
    if (file_index < 0 || file_index >= MAX_FILES) {
        printf("Error: Invalid file index.\n");
        return;
    }

    fat[file_index].size = 0;  // Mark the file as deleted in the FAT
}

int main() {
    init_disk();

    // Example usage
    int file_index = create_file("test.txt", 100);
    if (file_index != -1) {
        write_file(file_index, "Hello, World!");
        char buffer[100] = {0};
        read_file(file_index, buffer);
        printf("Read from file: %s\n", buffer);
        delete_file(file_index);
    }

    return 0;
}
