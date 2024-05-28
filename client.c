// Client side C program
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <ctype.h>
#include <arpa/inet.h> 
#include <sys/socket.h> // -> socket(), connect(), send(), recv()
#include <unistd.h>

int main(int argc, char const* argv[]){
    char input[2048], message[2048], response[2048];
    if(argc != 3) {
        printf("Usage: %s <ip> <port>\n", argv[0]);
        return 1;
    }
    char const* ip = argv[1];
    int port = atoi(argv[2]);

	int status, valrecv, client_fd;
	struct sockaddr_in serv_addr;
	if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("\n Socket creation error \n");
		return -1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	// Convert IPv4 and IPv6 addresses from text to binary form
	if (inet_pton(AF_INET, ip , &serv_addr.sin_addr) <= 0) {
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}

	if ((status = connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0) {
		printf("\nConnection Failed \n");
		return -1;
	}
    while(1){
        scanf("%[^\n]s", input); getchar();
        char* token = strtok(input, " "); 
        for (int i = 0; token[i]; i++) token[i] = toupper(token[i]);
        if(strcmp(token, "EXIT") == 0){
            // closing the connected socket
	        close(client_fd);
            break;
        }
        else if(strcmp(token, "GET") == 0){
            strcpy(message, "*2\r\n");
            while(token != NULL){ 
                char tmp[2048];
                sprintf(tmp, "$%zu\r\n%s\r\n", strlen(token), token);
                strcat(message,tmp);
                token = strtok(NULL, " ");
            }
            send(client_fd, message, strlen(message), 0);
            valrecv = recv(client_fd, response, 2048 - 1, 0); // subtract 1 for the null terminator at the end
            response[valrecv] = '\0';
            if(strcmp(response,"$-1\r\n")==0){
                printf("(nil)\n");
            }
            else{
                char* ptr=response;
                int len = 0;
                ptr++;
                while(*ptr != '\r') {
                    len = (len*10)+(*ptr - '0');
                    ptr++;
                }
                printf("\"%s\"", ptr+2);
            }
        }
        else if(strcmp(token, "SET") == 0){
            strcpy(message, "*3\r\n");
            while(token != NULL){ 
                char tmp[2048];
                sprintf(tmp, "$%zu\r\n%s\r\n", strlen(token), token);
                strcat(message,tmp);
                token = strtok(NULL, " ");
            }
            send(client_fd, message, strlen(message), 0);
            valrecv = recv(client_fd, response, 2048 - 1, 0); // subtract 1 for the null terminator at the end
            response[valrecv] = '\0';
            printf("\"%s\"", response);
        }
        else{
            printf("Invalid input\n");
            continue;
        }
    }
	return 0;
}
