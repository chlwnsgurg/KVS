// Server side C program
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char const* argv[]) {
    
    char* K[10];
    char* V[10]; 
    int top=-1;
    for (int i = 0; i < 10; i++) {
        K[i] = (char*)malloc(1024 * sizeof(char));
        V[i] = (char*)malloc(1024 * sizeof(char));
        if (K[i] == NULL || V[i] == NULL) {
            perror("malloc failed");
            return 1;
        }
        strcpy(K[i], ""); // initialize each element with an empty string
    }
    char message[2048],response[2048];
    if(argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return 1;
    }
    int port = atoi(argv[1]);
    int server_fd, conn_socket;
    ssize_t valrecv;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        return 1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        return 1;
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        return 1;
    }
    if ((conn_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen)) < 0) {
        perror("accept failed");
        return 1;
    }
    while(1){
        valrecv = recv(conn_socket, message, 1024 - 1, 0); // subtract 1 for the null terminator at the end
        message[valrecv] = '\0';
        // parsing the message  
        char* ptr=message;
        int len = 0;
        ptr++;
        while(*ptr != '\r') {
            len = (len*10)+(*ptr - '0');
            ptr++;
        }
        ptr+=3;
        char* command=strtok(ptr, "$");
        while(*command != '\r') command++;
        command+=2;
        command[strlen(command)-2]='\0';
        if(strcmp(command,"GET")==0){
           char* key=strtok(NULL, "$");
            while(*key!='\r') key++;
            key+=2;
            key[strlen(key)-2]='\0';
            int idx;
            for(idx=0;idx<10;idx++){
                if(strcmp(K[idx],key)==0) break;
            }
            if(idx==10) send(conn_socket, "$-1\r\n", 5, 0);
            else{
                sprintf(response, "$%zu\r\n%s\r\n", strlen(V[idx]), V[idx]);
                send(conn_socket, response, strlen(response), 0);
            }
        }
        if(strcmp(command,"SET")==0){
            char* key=strtok(NULL, "$");
            while(*key!='\r') key++;
            key+=2;
            key[strlen(key)-2]='\0';
            char* value=strtok(NULL, "$");
            while(*value!='\r') value++;
            value+=2;
            value[strlen(value)-2]='\0';
            ++top;
            strcpy(K[top],key);
            strcpy(V[top],value);
            send(conn_socket, "+OK\r\n", 5, 0);
            continue;
        }
    }
    // closing the connected socket
    close(conn_socket);
    // closing the listening socket
    close(server_fd);
    return 0;
}
