#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <string.h>

int main() {
    struct stat fileStat;
    struct passwd *pw;
    char filename[1024];
   
    // Read file names from stdin
    while (fgets(filename, sizeof(filename), stdin)) {
        filename[strcspn(filename, "\n")] = 0;  // Remove newline
       
        // Get file stats
        if (stat(filename, &fileStat) < 0) {
            continue;
        }
       
        // Get file owner
        pw = getpwuid(fileStat.st_uid);
       
        // Output: owner and file size
        printf("%s %lld\n", pw->pw_name, fileStat.st_size);
    }
   
    return 0;
}
