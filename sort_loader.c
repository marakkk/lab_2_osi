#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "block_cache.h"

void bubble_sort(int *arr, int n) {
    for (int i = 0; i < n - 1; i++) {
        printf("Iteration %d\n", i + 1);
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

void print_array(int *arr, int n) {
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}


void save_array_to_file(const char *file_path, int *arr, int n) {
    int fd = lab2_open(file_path);
    if (fd < 0) {
        perror("lab2_open");
        return;
    }

    if (lab2_write(fd, arr, n * sizeof(int)) < 0) {
        perror("lab2_write");
    }

    lab2_fsync(fd); 
    lab2_close(fd);
}


void load_array_from_file(const char *file_path, int *arr, int n) {
    int fd = lab2_open(file_path);
    if (fd < 0) {
        perror("lab2_open");
        return;
    }

    if (lab2_read(fd, arr, n * sizeof(int)) < 0) {
        perror("lab2_read");
    }

    lab2_close(fd);
}

