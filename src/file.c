#include "file.h"

char **readFileContent(FILE *file, int *numLines)
{

    // Count the number of lines in the file
    int lines = 0;
    int ch;
    while ((ch = fgetc(file)) != EOF)
    {
        if (ch == '\n')
        {
            lines++;
        }
    }
    fseek(file, 0, SEEK_SET);

    // Allocate memory for the array of strings
    char **content = (char **)malloc((lines + 1) * sizeof(char *)); // Account for the terminating NULL

    // Read the content of the file line by line
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int i = 0;
    while ((read = getline(&line, &len, file)) != -1)
    {
        content[i] = strdup(line); // Use strdup to copy the line
        line = NULL; // Reset the pointer
        i++;
    }
    content[i] = NULL; // Set the last pointer to NULL

    *numLines = lines; // Set the number of lines

    // Close the file
    fclose(file);

    return content;
}

long getFileSize(FILE *file)
{
    long size;
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, 0, SEEK_SET); // Reset the file position indicator to the beginning
    return size;
}

FileMetadata *handle_file(const char *filePath)
{
    FileMetadata *fd = malloc(sizeof(FileMetadata));
    int lines = 0;
    if (access(filePath, F_OK) != -1)
    {
        FILE *file = fopen(filePath, "r"); // Open the file in read mode
        if (file == NULL)
        {
            fprintf(stderr, "can't open the file\n");
            exit(EXIT_FAILURE);
        }

        fd->file = file;
        fd->file_name = strdup(filePath); // Use strdup to copy the file path
        fd->file_size = getFileSize(file);
        fd->file_path = strdup(filePath); // Use strdup to copy the file path
        fd->file_type = strdup("txt");
        fd->is_modified = 0;

        return fd;
    }
    else
    {
        free(fd);
        return NULL; // Return NULL instead of 0
    }
}
