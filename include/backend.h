#ifndef BACKEND_H
#define BACKEND_H

char *get_home(void);
int yes_no(const char *prompt);
int dir_exists(const char *directory);
int file_exists(const char *filename);
int delete_directory(const char *directory);
int make_directory(const char *directory);
int write_to_file(const char *filename, const char *input, const char *mode);
int move_file(const char *filename, const char *destination);
int starts_with(const char *string, const char *match);

#endif
