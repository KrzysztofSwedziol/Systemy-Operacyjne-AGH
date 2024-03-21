#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

int main() {
    DIR *currDir;
    currDir = opendir(".");
    if (currDir == NULL) {
        perror("opendir");
        return EXIT_FAILURE;
    }
    struct dirent *dir;
    struct stat buf;
    long long totalSize = 0;


    while ((dir = readdir(currDir)) != NULL) {

        if (stat(dir->d_name, &buf) != -1) {
            if (!S_ISDIR(buf.st_mode)) {
                printf("%lld\t%s\n", (long long)buf.st_size, dir->d_name);
                totalSize += buf.st_size;
            }
        } else {
            perror("stat");
        }
    }
    closedir(currDir);
    printf("Total size: %lld bytes\n", totalSize);
    return 0;
}
