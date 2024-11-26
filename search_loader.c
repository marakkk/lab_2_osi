#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "block_cache.h" 

int search_file(const char *dir_path, const char *target_name) {
    struct dirent *entry;
    DIR *dp = opendir(dir_path);
    if (dp == NULL) {
        perror("opendir");
        return -1;
    }

    while ((entry = readdir(dp)) != NULL) {
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;

            // Рекурсивный DFS поиск
            char new_path[1024];
            snprintf(new_path, sizeof(new_path), "%s/%s", dir_path, entry->d_name);
            if (search_file(new_path, target_name) == 1) {
                closedir(dp);
                return 1;
            }
        } else {
            if (strcmp(entry->d_name, target_name) == 0) {
                char file_path[1024];
                snprintf(file_path, sizeof(file_path), "%s/%s", dir_path, entry->d_name);

                int fd = lab2_open(file_path);
                if (fd < 0) {
                    perror("lab2_open");
                    closedir(dp);
                    return -1;
                }

                struct stat file_stat;
                if (fstat(fd, &file_stat) == 0) {
                    printf("Found '%s' (inode: %ld)\n", target_name, file_stat.st_ino);
                }

                lab2_close(fd); 
                closedir(dp);
                return 1;
            }
        }
    }

    closedir(dp);
    return 0;
}



