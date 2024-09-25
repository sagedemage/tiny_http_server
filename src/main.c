#include <dirent.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

enum {
    PORT = 8000,
    MAX_HTML_CHARACTER_LIMIT = 1000,
};

static struct sockaddr_in SetupAddress(int port) {
    /* Setup Address */
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    return address;
}

void SubStr(char* result, char* str, int start, int end) {
    /* Get a sub string of a char pointer */
    strncpy(result, str + start, end - start);
}

int IsDirectory(const char* path) {
    struct stat statbuf;
    if (stat(path, &statbuf) == 0) {
        return S_ISDIR(statbuf.st_mode);
    }
    return 0;
}

char* FindRequestedHtmlFile(char* file_route, char* path_route) {
    /* Recursively look through the directory to find the requested html file */
    DIR* d = NULL;
    struct dirent* dir = NULL;
    char* file_name = "static/404.html";

    // Open directory
    d = opendir(path_route);

    if (d) {
        while ((dir = readdir(d)) != NULL) {
            // append the path route with slash as the path
            char path[1025] = "";
            strncat(path, path_route, 300);
            strncat(path, "/", 300);

            // Skip dot and double dot
            if (strcmp(dir->d_name, ".") == 0) {
                continue;
            }
            if (strcmp(dir->d_name, "..") == 0) {
                continue;
            }

            strncat(path, dir->d_name, 300);

            if (IsDirectory(path) == 1) {
                file_name = FindRequestedHtmlFile(file_route, path);
            } else if (strcmp(file_route, path) == 0) {
                file_name = file_route;

                // Close directory
                closedir(d);
                return file_name;
            }
        }

        // Close directory
        closedir(d);
    }

    return file_name;
}

size_t ReadHtmlFile(const char* html_file_path, char** content) {
    /* Read html file and returns its buffer */
    // Keep track of the size of the content
    // buffer when characters are added to it
    size_t char_buf_size = MAX_HTML_CHARACTER_LIMIT;

    FILE* fptr = NULL;

    if (html_file_path == NULL) {
        printf("Error: html file path not specified.\n");
    }

    // Open file
    fptr = fopen(html_file_path, "r");

    if (fptr == NULL) {
        printf("Error: can't open file %s.\n", html_file_path);
    }

    char* content_buf =
        (char*)calloc(char_buf_size, char_buf_size * sizeof(char));
    // char* content_buf = (char*)malloc(char_buf_size * sizeof(content_buf));

    if (content_buf == NULL) {
        printf("Unable to perform allocation of the array.\n");
    }

    char line[100];

    size_t num_of_chars_after_concat = 0;
    char* temp = NULL;

    // Add line of the content to the content buffer
    while (fgets(line, 100, fptr)) {
        /* Get the number of chars after string
        concatination to check if it would cause a
        buffer overflow. If that is the case, the array
        must be reallocated to a bigger size to avoid
        a buffer overflow */

        num_of_chars_after_concat += 100;
        if (num_of_chars_after_concat > char_buf_size) {
            char_buf_size *= 2;
            temp = (char*)realloc(content_buf, char_buf_size * sizeof(char));

            if (temp != NULL) {
                content_buf = temp;
            } else {
                printf("Unable to reallocate that array to a bigger size.\n");
            }
        }

        // Issue here
        strncat(content_buf, line, 100);

        char_buf_size += 100;
    }

    *content = content_buf;

    // Close file
    int success = fclose(fptr);

    if (success != 0) {
        printf("Unable to close file!");
    }

    return char_buf_size;
}

int main(void) {
    int opt = 1;

    // Setup address
    struct sockaddr_in address = SetupAddress(PORT);
    socklen_t addrlen = sizeof(address);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd == -1) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    // Set the socket options
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) ==
        -1) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // Bind address to the socket
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == -1) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_fd, 3) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server is running\n");
    printf("at http://localhost:%i\n", PORT);
    printf("------------------------\n");

    while (true) {
        // Accept for connections
        int new_socket =
            accept(server_fd, (struct sockaddr*)&address, &addrlen);

        if (new_socket == -1) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        int read_buf[1024] = {0};

        // Read buffer of request
        ssize_t read_status = read(new_socket, read_buf, 1024);

        if (read_status == -1) {
            printf("Reading the message failed \n");
            return -1;
        }

        char* temp_p = (char*)read_buf;

        char temp_arr[1024] = {0};

        for (long unsigned int i = 0; i < strlen(temp_p); i++) {
            temp_arr[i] = temp_p[i];
        }

        char* line = strtok(temp_arr, "\n");

        char* request = "";

        while (line != NULL) {
            request = line;
            line = strtok(NULL, "\n");
            break;
        }

        // Seperate the request into a list by space
        char* request_items[3] = {0, 0, 0};
        int i = 0;

        line = strtok(request, " ");
        while (line != NULL) {
            request_items[i] = line;
            line = strtok(NULL, " ");
            ++i;
        }

        // change file route
        char* file_route = request_items[1];
        char static_folder[100] = "static";
        file_route = strncat(static_folder, file_route, 50);
        char file_route_last_char = file_route[strlen(file_route) - 1];

        // file with .ico file extension
        int length = (int)strlen(file_route);

        char ico_ext_result[1025] = "";
        SubStr(ico_ext_result, file_route, length - 4, length);
        char* ico_extension = ico_ext_result;

        // file with .html file extension
        char html_ext_result[1025] = "";
        SubStr(html_ext_result, file_route, length - 5, length);
        char* html_extension = html_ext_result;

        if (file_route_last_char == '/') {
            strncat(file_route, "index.html", 100);
        } else if (strcmp(html_extension, ".html") != 0 &&
                   strcmp(ico_extension, ".ico") != 0) {
            strncat(file_route, "/index.html", 100);
        }

        // find if requested file exists in web server
        const char* file_name = FindRequestedHtmlFile(file_route, "static");

        printf("File name: %s\n", file_name);

        // read HTML file
        char* buf = NULL;
        size_t char_buf_size = ReadHtmlFile(file_name, &buf);

        // Store HTML char pointer data
        char* response = calloc(char_buf_size + 100, sizeof(char));
        strncpy(response, "HTTP/1.1 200 OK\n\n", 100);
        //-response = "HTTP/1.1 200 OK\n\n";
        //-char response[char_buf_size + 100] = "HTTP/1.1 200 OK\n\n";
        char* html = strncat(response, buf, MAX_HTML_CHARACTER_LIMIT);

        // free memory
        free(response);
        free(buf);

        // Send the buffer of html web page
        ssize_t send_status = send(new_socket, html, strlen(html), 0);

        if (send_status != -1) {
            printf("%s", (char*)read_buf);
        } else {
            printf("Sending the message failed \n");
            return -1;
        }

        // close the socket
        close(new_socket);
    }
}
