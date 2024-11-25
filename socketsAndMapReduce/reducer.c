#include <stdio.h>
#include <string.h>

int main() {
    char owner[1024], current_owner[1024];
    long size, max_size = 0;
   
    // Read owner and file size pairs from stdin
    while (scanf("%s %ld", current_owner, &size) != EOF) {
        if (size > max_size) {
            max_size = size;
            strcpy(owner, current_owner);
        }
    }
   
    // Output the user with the largest file
    printf("User: %s, Largest File Size: %ld\n", owner, max_size);
   
    return 0;
}