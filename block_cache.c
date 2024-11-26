#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <pthread.h>


#define BLOCK_SIZE 4096  
#define CACHE_SIZE 16    

typedef struct CacheBlock {
    int fd;
    off_t offset;
    char data[BLOCK_SIZE];
    int is_dirty;
    int last_access;
} CacheBlock;

static CacheBlock cache[CACHE_SIZE];
static int cache_clock = 0;
pthread_mutex_t cache_mutex = PTHREAD_MUTEX_INITIALIZER;


static int find_block(int fd, off_t offset) {
    for (int i = 0; i < CACHE_SIZE; i++) {
        if (cache[i].fd == fd && cache[i].offset == offset) {
            return i;
        }
    }
    return -1;
}


static int evict_block() {
    int oldest_index = 0;
    for (int i = 1; i < CACHE_SIZE; i++) {
        if (cache[i].last_access < cache[oldest_index].last_access) {
            oldest_index = i;
        }
    }
    if (cache[oldest_index].is_dirty) {
        pwrite(cache[oldest_index].fd, cache[oldest_index].data, BLOCK_SIZE, cache[oldest_index].offset);
    }
    return oldest_index;
}


int lab2_open(const char *path) {
    return open(path, O_RDWR);
}


int lab2_close(int fd) {
    pthread_mutex_lock(&cache_mutex);
    for (int i = 0; i < CACHE_SIZE; i++) {
        if (cache[i].fd == fd) {
            if (cache[i].is_dirty) {
                pwrite(cache[i].fd, cache[i].data, BLOCK_SIZE, cache[i].offset);
            }
            cache[i].fd = -1;
        }
    }
    pthread_mutex_unlock(&cache_mutex);
    return close(fd);
}


ssize_t lab2_read(int fd, void *buf, size_t count) {
    off_t offset = lseek(fd, 0, SEEK_CUR);
    int block_index = find_block(fd, offset / BLOCK_SIZE);
    if (block_index != -1) {
        memcpy(buf, cache[block_index].data + (offset % BLOCK_SIZE), count);
        cache[block_index].last_access = ++cache_clock;
        return count;
    }

    pthread_mutex_lock(&cache_mutex);
    block_index = find_block(fd, offset / BLOCK_SIZE);
    if (block_index == -1) {
        block_index = evict_block();
        cache[block_index].fd = fd;
        cache[block_index].offset = offset / BLOCK_SIZE;
        pread(fd, cache[block_index].data, BLOCK_SIZE, cache[block_index].offset * BLOCK_SIZE);
        cache[block_index].is_dirty = 0;
    }
    pthread_mutex_unlock(&cache_mutex);

    memcpy(buf, cache[block_index].data + (offset % BLOCK_SIZE), count);
    cache[block_index].last_access = ++cache_clock;
    return count;
}


ssize_t lab2_write(int fd, const void *buf, size_t count) {
    off_t offset = lseek(fd, 0, SEEK_CUR);
    pthread_mutex_lock(&cache_mutex);
    int block_index = find_block(fd, offset / BLOCK_SIZE);
    if (block_index == -1) {
        block_index = evict_block();
        cache[block_index].fd = fd;
        cache[block_index].offset = offset / BLOCK_SIZE;
    }
    memcpy(cache[block_index].data + (offset % BLOCK_SIZE), buf, count);
    cache[block_index].is_dirty = 1;
    cache[block_index].last_access = ++cache_clock;
    pthread_mutex_unlock(&cache_mutex);
    return count;
}


off_t lab2_lseek(int fd, off_t offset, int whence) {
    return lseek(fd, offset, whence);
}


int lab2_fsync(int fd) {
    pthread_mutex_lock(&cache_mutex);
    for (int i = 0; i < CACHE_SIZE; i++) {
        if (cache[i].fd == fd && cache[i].is_dirty) {
            pwrite(cache[i].fd, cache[i].data, BLOCK_SIZE, cache[i].offset * BLOCK_SIZE);
            cache[i].is_dirty = 0;
        }
    }
    pthread_mutex_unlock(&cache_mutex);
    return fsync(fd);
}

