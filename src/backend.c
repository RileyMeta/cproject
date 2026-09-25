#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include "backend.h"

char *get_home(void) {
    return getenv("HOME");
}

int yes_no(const char *prompt) {
    char input[8];

    while (1) {
        printf("%s", prompt);
        fgets(input, sizeof(input), stdin);

        // Sloppy, we're only checking if the input starts with 'y' or 'n'
        // This helps with mis-types (which I do a lot)
        char response = tolower(input[0]);

        if (response == 'y') {
            return 0;
        } else if (response == 'n') {
            return 1;
        } else {
            printf("Response '%c' is unknown, please try again.\n", response);
            continue;
        }
    }

    return 1; // Default to no to prevent data loss
}

int dir_exists(const char *directory) {
    if (access(directory, F_OK) == 0) {
        return 0;
    }

    return 1;
}

int file_exists(const char *filename) {
    if (access(filename, F_OK) == 0) {
        return 0;
    }
    return 1;
}

int delete_directory(const char *directory) {
    DIR *dir = opendir(directory);
    if (!dir) {
        perror("opendir");
        return -1;
    }

    struct dirent *entry;
    char full_path[1024];

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        snprintf(full_path, sizeof(full_path), "%s/%s", directory, entry->d_name);

        if (entry->d_type == 4) {
            if (delete_directory(full_path) == -1) {
                closedir(dir);
                return -1;
            }
        } else {
            if (remove(full_path) == -1) {
                closedir(dir);
                return -1;
            }
        }
    }

    if (rmdir(directory) == -1) {
        perror("rmdir");
        closedir(dir);
        return -1;
    }

    closedir(dir);
    return 0;
}

int make_directory(const char *directory) {
    return mkdir(directory, 0700);
}

int write_to_file(const char *filename, const char *input, const char *mode) {
    FILE *f;

    f = fopen(filename, mode);

    if (f == NULL) {
        perror("Error opening file");
        return 1;
    }

    fprintf(f, input);
    fclose(f);

    return 0;
}

int move_file(const char *filename, const char *destination) {
    if (rename(filename, destination) != 0) {
        perror("Error renaming file");
        return 1;
    }

    return 0;
}

int ends_with(const char *string, const char *match) {
    size_t string_len = strlen(string);
    size_t match_len = strlen(match);

    if (match_len > string_len) {
        return 1;
    }

    int index = string_len - match_len;

    for (int i = index, o = 0; i < string_len; i++, o++) {
        if (string[i] != match[o]) {
            return 1;
        }
    }

    return 0;
}

int starts_with(const char *string, const char *match) {
    size_t string_len = strlen(string);
    size_t match_len =  strlen(match);

    if (match_len > string_len) {
        return 1;
    }

    for (int i = 0; i < match_len; i++) {
        if (string[i] != match[i]) {
            return 1;
        }
    }

    return 0;
}

int empty_dir(const char *dirname) {
    int n = 0;
    struct dirent *d;

    DIR *dir = opendir(dirname);

    if (dir == NULL) {
        //Not a directory or doesn't exist
        return 2;
    }

    while ((d = readdir(dir)) != NULL) {
        if (++n > 2) {
            break;
        }
    }

    closedir(dir);

    if (n <= 2) {
        //Directory Empty
        return 0;
    }

    // Not Empty
    return 1;
}

int walk_directory(const char *dir_path, char ***arr) {
    struct dirent *entry;
    struct stat statbuf;
    size_t count = 0;
    size_t capacity = 0;

    DIR *dp = opendir(dir_path);

    if (dp == NULL) {
        perror("opendir");
        return -1;
    }

    while ((entry = readdir(dp)) != NULL) {
        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", dir_path, entry->d_name);

        // Skip the special entries "." and ".."
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            if (stat(path, &statbuf) == -1) {
                perror("stat");
                continue;
            }

            // Add to array
            if (count == capacity) {
                capacity += 16;
                *arr = realloc(*arr, capacity * sizeof(char *));
                if (*arr == NULL) {
                    perror("realloc");
                    closedir(dp);
                    return -1;
                }
            }
            (*arr)[count++] = strdup(path);
        }
    }

    closedir(dp);
    return count;
}

char *r_split(char *string, const char delim) {
    char *return_str = malloc(sizeof(char) * 256);
    char output[256];
    int length = strlen(string) - 1;
    int start = 0;

    for (int i = length; i >= 0; i--) {
        if (string[i] == delim) {
            start = i + 1;
            break;
        }
    }

    if (start == 0) {
        return string;
    }

    for (int i = start, o = 0; i <= length; i++, o++) {
        output[o] = string[i];
    }

    strcpy(return_str, output);
    return return_str;
}
