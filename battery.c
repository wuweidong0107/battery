#include <dirent.h>
#include <err.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>

#ifndef MAX_PATH
#define MAX_PATH    260
#endif

#define SYS_NODE    "/sys/class/power_supply"

struct device {
    char *path;
};

static int read_sysfs(const char* filename, char* buf, size_t size)
{
    int ret;
    FILE* f = fopen(filename, "r");
    if(!f)
        return -1;
    
    ret = fread(buf, 1, size-1, f);
    fclose(f);

    if (ret <= 0)
        return -1;
    
    buf[ret] = 0;

    return 0;
}

static int dev_init(struct device *dev)
{
    DIR *dir;
    struct dirent *d;
    size_t len;
    int found = 0;
    char capacity[MAX_PATH];

    dir = opendir(SYS_NODE);
    if (!dir)
        return 0;

    while((d = readdir(dir))) {
        if (d->d_name[0] == '.')
            continue;
        len = strlen(SYS_NODE) + strlen(d->d_name) + strlen("capacity") + 16;
        snprintf(capacity, len, "%s/%s/capacity", SYS_NODE, d->d_name);
        
        if (access(capacity, R_OK) == 0) {
            found = 1;
            len = strlen(SYS_NODE) + strlen(d->d_name) + 2;
            dev->path = malloc(len);
            if (!dev->path)
                return 0;
            snprintf(dev->path, len, "%s/%s", SYS_NODE, d->d_name);
            break;
        }
    }
    closedir(dir);
    return found ? 0:1;
}

int main(int argc, char *argv[])
{
    struct device dev;

    if (dev_init(&dev)) 
        err(1, "System has no battery");
    
    //printf("Found: %s\n", dev.path);
    char path[256] = {0};
    char capacity[64] = {0};
    char status[64] = {0};

    snprintf(path, sizeof(path), "%s/capacity", dev.path);
    read_sysfs(path, capacity, sizeof(capacity));

    snprintf(path, sizeof(path), "%s/status", dev.path);
    read_sysfs(path, status, sizeof(status));

    printf("capacity: %s", capacity);
    printf("status: %s", status);
}