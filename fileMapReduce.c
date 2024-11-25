#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
struct FileOwner
{
    char *owner;
    off_t size;
};
// Function to get the file owner
char *get_file_owner(const char *path)
{
    struct stat fileStat;
    struct passwd *pw;
    if (stat(path, &fileStat) == 0)
    {
        pw = getpwuid(fileStat.st_uid);
        return pw->pw_name;
    }
    return NULL;
}
// Function to find maximum file size and owner
void find_max_file(const char *dir_name)
{
    DIR *dir;
    struct dirent *entry;
    struct stat fileStat;
    struct FileOwner max_file = {NULL, 0};
    dir = opendir(dir_name);
    if (dir == NULL)
    {
        perror("Unable to open directory");
        exit(EXIT_FAILURE);
    }
    while ((entry = readdir(dir)) != NULL)
    {
        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", dir_name,
                 entry->d_name);
        if (stat(path, &fileStat) == 0 &&
            S_ISREG(fileStat.st_mode))
        { // Only consider regular files 
        char *owner = get_file_owner(path);
            if (fileStat.st_size > max_file.size)
            {
                max_file.owner = owner;
                max_file.size = fileStat.st_size;
            }
        }
    }
    closedir(dir);
    if (max_file.owner != NULL)
    {
        printf("User owning the file with maximum size: % s\n ",max_file.owner);
        printf("Maximum file size: %lld bytes\n",max_file.size);
    }
    else
    {
        printf("No files found in the directory.\n");
    }
}
int main()
{
    find_max_file("."); // Current working directory
    return 0;
}