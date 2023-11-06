#include <netinet/in.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>

#define PORT 8000
#define MAX_HTML_CHARACTER_LIMIT 10000

static struct sockaddr_in setup_address() {
	/* Setup Address */
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);
	return address;
}

char* sub_str(char* str, int start, int end) {
	/* Get a sub string of a char pointer */
	char result[25] = "";

	strncpy(result, str + start, end - start);

	char* result_ptr = result;
	return result_ptr;
}

int is_directory(const char* path) {
	struct stat statbuf;
	if (stat(path, &statbuf) == 0) {
		return S_ISDIR(statbuf.st_mode);
	}
	return 0;
}

char* find_requested_html_file(char* file_route, char* path_route) {
	/* Recursively look through the directory to find the requested html file */
	DIR *d;
	struct dirent *dir;
	char* file_name = "static/404.html";

	// Open directory
	d = opendir(path_route);

	if (d) {
		while ((dir = readdir(d)) != NULL) {
			// Skip dot and double dot
			if (strcmp(dir->d_name, ".") == 0) {
				continue;
			}
			if (strcmp(dir->d_name, "..") == 0) {
				continue;
			}

			if (is_directory(dir->d_name) == 1) {
				file_name = find_requested_html_file(file_route, dir->d_name);
			}
			if (strcmp(file_route, dir->d_name) == 0) {
				file_name = dir->d_name;
			}
		}

		// Close directory
		closedir(d);
	}

	return file_name;
}

char* read_html_file(char* html_file_path) {
	/* Read html file and returns its buffer */
	FILE *fptr;

	// Open file
	fptr = fopen(html_file_path, "r");

	char content_buf[MAX_HTML_CHARACTER_LIMIT] = "";
	char line[100];

	// Add line of the content to the content buffer
	while (fgets(line, 100, fptr)) {
		strncat(content_buf, line, 100);
	}
	
	// Close file
	fclose(fptr);

	// Convet char array to pointer
	char* content_buf_ptr = content_buf;

	return content_buf_ptr;
}

int main() {
	int opt = 1;

	// Setup address
	struct sockaddr_in address = setup_address();
	socklen_t addrlen = sizeof(address);

	int server_fd = socket(AF_INET, SOCK_STREAM, 0);

	if (server_fd == -1) {
		perror("socket failed");
		exit(EXIT_FAILURE);

	}
	// Set the socket options
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
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
	printf("-----------------\n");

	while (true) {
		// Accept for connections
		int new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);

		if (new_socket == -1) {
			perror("accept");
			exit(EXIT_FAILURE);
		}

		int read_buf[1024] = { 0 };

		// Read buffer of request
		ssize_t read_status = read(new_socket, read_buf, 1024);

		if (read_status == -1) {
			printf("Reading the message failed \n");
			return -1;
		}

		char* temp2 = (char*)read_buf;

		char temp3[1024] = { 0 };

		for (long unsigned int i = 0; i < strlen(temp2); i++) {
			temp3[i] = temp2[i];
		}

		char *line = strtok(temp3, "\n");

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
		char file_route_last_char = file_route[strlen(file_route) -1];
		// file with .ico file extension
		int length = strlen(file_route);
		char* ico_extension = sub_str(file_route, length - 4, length); 
		//file_route.substr(strlen(file_route)-4, -1);
		// file with .html file extension
		char* html_extension = sub_str(file_route, length - 5, length);
		//file_route.substr(strlen(file_route)-5, -1);

		if (file_route_last_char == '/') {
			strncat(file_route, "index.html", 100);
		}
		else if (strcmp(html_extension, ".html") != 0 && strcmp(ico_extension, ".ico") != 0) {
			strncat(file_route, "/index.html", 100);
		}
		

		// find if requested file exists in web server
		char* file_name = find_requested_html_file(file_route, "static");

		// read HTML file
		char* buf = read_html_file(file_name);

		// Store HTML char pointer data
		char response[MAX_HTML_CHARACTER_LIMIT+100] = "HTTP/1.1 200 OK\n\n";
		char* html = strncat(response, buf, MAX_HTML_CHARACTER_LIMIT);

		// Send the buffer of html web page
		ssize_t send_status = send(new_socket, html, strlen(html), 0);

		if (send_status == -1) {
			printf("Sending the message failed \n");
			return -1;
		}
		else {
			printf("%s", (char*)read_buf);
		}

		// close the socket
		close(new_socket);
	}
}
