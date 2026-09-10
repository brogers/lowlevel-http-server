#include <stdlib.h>
#include <util.h>

char *loadfile(char *path) {
  FILE *file = fopen(path, "rb+");
  if (!file) {
    perror("Failed to open file");
    exit(EXIT_FAILURE);
  }

  fseek(file, 0, SEEK_END);
  size_t file_size = (size_t)ftell(file);
  fseek(file, 0, SEEK_SET);

  char *file_content = malloc(file_size + 1);
  if (!file_content) {
    perror("Failed to allocate memory for file content");
    fclose(file);
    exit(EXIT_FAILURE);
  }

  fread(file_content, 1, file_size, file);
  fclose(file);
  file_content[file_size] = '\0';
  return file_content;
}
