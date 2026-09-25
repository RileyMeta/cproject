#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "backend.h"

void usage(void);
void help_menu(void);
void version_menu(void);
int rearrange_project(const char *project_name);
int create_project(const char *project_name);
int make_readme(const char *filename);
int make_subdir(const char *project_name, const char *subdir);
int make_c_file(const char *project_name, const char *subdir, const char *filename);
int make_makefile(const char *project_name);

// Global Variables
int verbose = 0;

int main(int argc, char *argv[]) {
    int makefile = 0;
    int in_place = 0;
    char **inputs;
    size_t capacity = 0;
    size_t count = 0;

    for (int i = 1; i < argc; i++) {
        char *cur_arg = argv[i];
        if ((cur_arg[0] == '-') || (cur_arg[0] == '-' && cur_arg[1] == '-')) {

            if (strcmp(cur_arg, "--help") == 0) {
                help_menu();
                return 0;
            } else if (strcmp(cur_arg, "-v") == 0 || strcmp(cur_arg, "--version") == 0) {
                version_menu();
                return 0;
            } else if (strcmp(cur_arg, "-M") == 0 || strcmp(cur_arg, "--makefile") == 0) {
                makefile = 1;
            } else if (strcmp(cur_arg, "-V") == 0 || strcmp(cur_arg, "--verbose") == 0) {
                verbose = 1;
            } else if (strcmp(cur_arg, "-I") == 0 || strcmp(cur_arg, "--inplace") == 0) {
                in_place = 1;
            } else {
                printf("Unknown argument: '%s'\n", cur_arg);
            }
        } else {
            if (count == capacity) {
                capacity += 16;
                inputs = realloc(inputs, capacity * sizeof(char *));
            }
            inputs[count++] = cur_arg;
        }
    }

    if (count < 1) {
        fprintf(stderr, "At least one (1) project name is required.\n");
        usage();
        return 1;
    }

    for (int i = 0; i < count; i++) {
        char *cur_in = inputs[i];

        if (makefile == 1) {
            make_makefile(cur_in);
        } else {
            if (in_place == 1) {
                rearrange_project(cur_in);
            } else {
                create_project(cur_in);
            }
        }
    }

    return 0;
}

void usage(void) {
    printf("Usage: cproject [OPTION] name...\n");
}

void help_menu(void) {
    usage();
    printf("Generate a templated C project folder.\n");
    printf("\n");
    printf("  -M, --makefile    create only a makefile\n");
    printf("  -I, --inplace     use the current folder as-is\n");
    printf("  -V, --verbose     print more debugging info\n");
    printf("\n");
    printf("      --help     display this help and exit\n");
    printf("      --version  output version information and exit\n");
    printf("\n");
    printf("Report bugs to: <https://github.com/RileyMeta/project_scripts/issues/>\n");
    printf("Project scripts home page: <https://github.com/RileyMeta/project_scripts/>\n");
}

void version_menu(void) {
    printf("cproject (C Project Templates) 1.0.0\n");
    printf("Copyright (C) 2026 Riley Ava.\n");
    printf("License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>.\n");
    printf("This is free software: you are free to change and redistribute it.\n");
    printf("There is NO WARRANTY, to the extent permitted by law.\n");
    printf("\n");
    printf("Written by Riley Ava.\n");
}

// Actual Logic
int rearrange_project(const char *project_name) {
    if (dir_exists(project_name) == 0) {
        printf("'%s' already exists.\n", project_name);
        printf("The contents of the folder will be rearranged.\n");
    } else {
        printf("'%s' does not exist.\n", project_name);
        printf("A new folder will be created.\n");
        return create_project(project_name);
    }

    // make sure src/ dir exists
    char src[256];
    strcpy(src, project_name);
    strcat(src, "/src/");
    if (dir_exists(src) != 0) {
        if (make_directory(src) != 0) {
            fprintf(stderr, "Unable to create directory '%s'.\n", src);
            return 1;
        }
    }

    // make sure include/ dir exists
    char include[256];
    strcpy(include, project_name);
    strcat(include, "/include/");
    if (dir_exists(include) != 0) {
        if (make_directory(include) != 0) {
            fprintf(stderr, "Unable to create directory '%s'.\n", include);
            return 1;
        }
    }

    // make sure lib/ dir exists
    char lib[256];
    strcpy(lib, project_name);
    strcat(lib, "/lib/");
    if (dir_exists(lib) != 0) {
        if (make_directory(lib) != 0) {
            fprintf(stderr, "Unable to create directory '%s'.\n", lib);
            return 1;
        }
    }

    // make sure src/main.c exists
    char mainc[256];
    strcpy(mainc, project_name);
    strcat(mainc, "/src/main.c");
    if (file_exists(mainc) != 0) {
        if (make_c_file(project_name, "src", "main.c") != 0) {
            fprintf(stderr, "Unable to create '%s'.\n", mainc);
            return 1;
        }
    }

    // make sure README.md exists
    char readme[256];
    strcpy(readme, project_name);
    strcat(readme, "/README.md");
    if (file_exists(readme) != 0) {
        if (make_readme(readme) != 0) {
            fprintf(stderr, "Unable to create '%s'.\n", readme);
            return 1;
        }
    }

    // make sure Makefile exists
    char makefile[256];
    strcpy(makefile, project_name);
    strcat(makefile, "/Makefile");
    if (file_exists(makefile) != 0) {
        if (make_makefile(makefile) != 0) {
            fprintf(stderr, "Unable to create '%s'.\n", makefile);
            return 1;
        }
    }

    char **files = NULL;

    if (verbose == 1) {
        printf("Walking Directory '%s'.\n", project_name);
    }
    size_t count = walk_directory(project_name, &files); // This is failing.

    if (count < 0) {
        printf("No files found inside '%s'\n", project_name);
    } else {

        printf("Files Found: %ld\n", count);
        // Walk the directory and move any files
        for (int i = 0; i < count; i++) {
            printf("'%s'\n", files[i]);

            // If any file ends with '.c' move to src/
            if (ends_with(files[i], ".c") == 0) {
                char dest[256];
                strcpy(dest, src);
                strcat(dest, "/");
                strcat(dest, files[i]);

                if (move_file(files[i], dest) != 0) {
                    fprintf(stderr, "Unable to move '%s' to '%s'\n", files[i], dest);
                }
            }

            // If any file ends with '.h' move to include/
            if (ends_with(files[i], ".h") == 0) {
                char dest[256];
                strcpy(dest, include);
                strcat(dest, "/");
                strcat(dest, files[i]);

                if (move_file(files[i], dest) != 0) {
                    fprintf(stderr, "Unable to move '%s' to '%s'\n", files[i], dest);
                }
            }
        }

        for (int i = 0; i < count; i++) {
            free(files[i]);
        }
        free(files);
    }

    printf("'%s' has been converted into a C project.\n", project_name);

    return 0;
}

int create_project(const char *project_name) {
    if (dir_exists(project_name) == 0) {
        printf("'%s' already exists.\n", project_name);
        printf("Would you like to overwrite it?\n");
        printf("[NOTE!] This will delete the folder and it's contents.\n");

        // Prompt user for confirmation
        if (yes_no("[Y]es or [N]o: ") != 0) {
            printf("'%s' has not been changed.\n", project_name);
            return 1;
        }

        // Delete directory
        if (delete_directory(project_name) != 0) {
            fprintf(stderr, "Unable to delete '%s'.\n", project_name);
            return 1;
        }

        if (verbose == 1) {
            printf("'%s' has been deleted.\n", project_name);
        }
    }

    // Create directory
    if (make_directory(project_name) != 0) {
        fprintf(stderr, "Unable to create directory '%s'.\n", project_name);
        return 1;
    }

    if (verbose == 1) {
        printf("'%s' directory created.\n", project_name);
    }

    // Create README.md
    if (make_readme(project_name) != 0) {
        fprintf(stderr, "Unable to create README.md.\n");
        return 1;
    }

    // Create include/
    if (make_subdir(project_name, "include") != 0) {
        fprintf(stderr, "Unable to create 'include/' directory.\n");
        return 1;
    }

    // Create lib/
    if (make_subdir(project_name, "lib") != 0) {
        fprintf(stderr, "Unable to create 'lib/' directory.\n");
        return 1;
    }

    // Create src/
    if (make_subdir(project_name, "src") != 0) {
        fprintf(stderr, "Unable to create 'src/' directory.\n");
        return 1;
    }

    // Create src/main.c
    if (make_c_file(project_name, "src", "main.c") != 0) {
        fprintf(stderr, "Unable to create 'src/main.c'.\n");
        return 1;
    }

    // Create Makefile
    if (make_makefile(project_name) != 0) {
        fprintf(stderr, "Unable to create the Makefile.\n");
        return 1;
    }

    printf("The '%s' c project template has been created.\n", project_name);

    return 0;
}

int make_readme(const char *project_name) {
    char main_file[256] = "";
    strcat(main_file, project_name);
    strcat(main_file, "/");
    strcat(main_file, "README.md");

    char header[256] = "# ";
    strcat(header, project_name);

    if (write_to_file(main_file, header, "w") != 0) {
        fprintf(stderr, "Unable to create '%s'.\n", main_file);
        return 1;
    }

    if (verbose == 1) {
        printf("'%s' has been created.\n", main_file);
    }

    return 0;
}

int make_subdir(const char *project_name, const char *subdir) {
    char subdir_name[256] = "";

    strcpy(subdir_name, project_name);
    strcat(subdir_name, "/");
    strcat(subdir_name, subdir);

    if (make_directory(subdir_name) != 0) {
        fprintf(stderr, "Unable to create '%s'.\n", subdir_name);
        return 1;
    }

    if (verbose == 1) {
        printf("'%s' sub-directory has been created.\n", subdir_name);
    }

    return 0;
}

int make_c_file(const char *project_name, const char *subdir, const char *filename) {
    char main_file[256] = "";
    strcat(main_file, project_name);
    strcat(main_file, "/");
    strcat(main_file, subdir);
    strcat(main_file, "/");
    strcat(main_file, filename);

    char *contents[] = {
        "#include <stdio.h>\n",
        "\n",
        "int main(int argc, char *argv[]) {\n",
        "    printf(\"Hello, World\");\n",
        "\n",
        "    return 0;\n",
        "}",
    };

    for (int i = 0; i < sizeof(contents) / sizeof(contents[0]); i++) {
        if (write_to_file(main_file, contents[i], "a") != 0) {
            fprintf(stderr, "Unable to write to '%s'.\n", main_file);
            return 1;
        }
    }

    if (verbose == 1) {
        printf("'%s' has been created.\n", main_file);
    }

    return 0;
}

int make_makefile(const char *project_name) {
    char main_file[256] = "";
    strcat(main_file, project_name);
    strcat(main_file, "/");
    strcat(main_file, "Makefile");

    char header[256] = "TARGET = ";
    strcat(header, project_name);
    strcat(header, "\n");

    char *contents[] = {
        header,
        "SRC := $(wildcard src/*.c)\n",
        "\n",
        "# Compiler and Flags\n",
        "CC = gcc\n",
        "CFLAGS = -Wall -std=c99 -Wno-missing-braces -Iinclude\n",
        "\n",
        "# Libraries and linking\n",
        "# LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11\n",
        "\n",
        "# Build rule\n",
        "$(TARGET): $(SRC)\n",
        "    $(CC) $(SRC) -o $(TARGET) $(CFLAGS) $(LIBS)\n",
        "\n",
        "# Clean rule\n",
        "clean:\n",
        "    rm -f $(TARGET)\n",
        "\n",
        "# Run rule\n",
        "run: $(TARGET)\n",
        "    ./$(TARGET)",
    };

    for (int i = 0; i < sizeof(contents) / sizeof(contents[0]); i++) {
        if (write_to_file(main_file, contents[i], "a") != 0) {
            fprintf(stderr, "Unable to write to '%s'.\n", main_file);
            return 1;
        }
    }

    if (verbose == 1) {
        printf("'%s' has been created.\n", main_file);
    }

    return 0;
}
