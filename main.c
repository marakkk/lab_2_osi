#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "block_cache.h"
#include "block_cache.c"
#include "search_loader.h"
#include "sort_loader.h"
#include "search_loader.c"
#include "sort_loader.c"

void run_search(const char *dir_path, const char *target_name) {
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);

    int result = search_file(dir_path, target_name);

    clock_gettime(CLOCK_REALTIME, &end);
    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    if (result == 1) {
        printf("Found '%s' in directory '%s'\n", target_name, dir_path);
    } else {
        printf("File '%s' not found in directory '%s'\n", target_name, dir_path);
    }

    printf("Execution time: %.3f seconds\n", elapsed);
}

void run_sort(int array_size, int iterations, const char *output_file) {
    int *arr = (int *)malloc(array_size * sizeof(int));
    if (arr == NULL) {
        perror("Failed to allocate memory");
        return;
    }

    srand(time(NULL));

    for (int iter = 0; iter < iterations; iter++) {
        for (int i = 0; i < array_size; i++) {
            arr[i] = rand() % 1000;
        }

        printf("Before sorting:\n");
        print_array(arr, array_size);

        struct timespec start, end;
        clock_gettime(CLOCK_REALTIME, &start);

        bubble_sort(arr, array_size);

        clock_gettime(CLOCK_REALTIME, &end);
        double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
        printf("Sorting time: %.3f seconds\n", elapsed);

        printf("After sorting:\n");
        print_array(arr, array_size);

        save_array_to_file(output_file, arr, array_size);
    }

    free(arr);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage:\n");
        printf("  %s search <directory_path> <file_name>\n", argv[0]);
        printf("  %s sort <array_size> <iterations> <output_file>\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "search") == 0) {
        if (argc != 4) {
            printf("Usage: %s search <directory_path> <file_name>\n", argv[0]);
            return 1;
        }
        const char *dir_path = argv[2];
        const char *target_name = argv[3];
        run_search(dir_path, target_name);
    } else if (strcmp(argv[1], "sort") == 0) {
        if (argc != 5) {
            printf("Usage: %s sort <array_size> <iterations> <output_file>\n", argv[0]);
            return 1;
        }
        int array_size = atoi(argv[2]);
        int iterations = atoi(argv[3]);
        const char *output_file = argv[4];
        run_sort(array_size, iterations, output_file);
    } else {
        printf("Invalid command. Use 'search' or 'sort'.\n");
        return 1;
    }

    return 0;
}

