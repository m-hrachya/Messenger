#include "lib.h"

int server_fd;
int client_sock_fds[MAX_CLIENT_COUNT];
int client_count;


char client_names[MAX_CLIENT_COUNT][NAME_SIZE];
char receve_buffer[BUFFER_SIZE] = {0};
char send_buffer[BUFFER_SIZE] = {0};

int addElement(int element) {
    for (int i = 0; i < MAX_CLIENT_COUNT; ++i) {
        if (client_sock_fds[i] == -1){
            client_sock_fds[i] = element;
            return i;
        } 
    }
    return -1;
}

void * write_thread_fnc(void * arg) {
    int * index = (int *) arg;
    while (1) {
        recv(client_sock_fds[*index], receve_buffer, BUFFER_SIZE, 0);
        if (!strcmp(receve_buffer, "exit\n")) {
            client_sock_fds[*index] = -1;
            --client_count;
            memset(client_names[*index], 0, NAME_SIZE);
            free(index);
            printf("User disconnected.\n");
            return NULL;
        }
        strcpy(send_buffer, client_names[*index]);
        strcat(send_buffer, ": ");
        strcat(send_buffer, receve_buffer);
        for (int i = 0; i < MAX_CLIENT_COUNT; ++i) {
            if (i == *index || client_sock_fds[i] == -1) continue;
            write(client_sock_fds[i], send_buffer, BUFFER_SIZE);
        }
        memset(receve_buffer, 0, BUFFER_SIZE);
        memset(send_buffer, 0, BUFFER_SIZE);
    }
    return NULL;
}

int main(int argc, char * argv[]) { 
    for (int i = 0; i < MAX_CLIENT_COUNT; ++i) {
        client_sock_fds[i] = -1;
    }  
    pthread_t write_thread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);


    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    assert(server_fd >= 0);
    address.sin_family = AF_INET;
    address.sin_port = htons(atoi(argv[1]));
    address.sin_addr.s_addr = INADDR_ANY;
    assert(bind(server_fd, (struct sockaddr *)&address, addrlen) >= 0);
    assert(listen(server_fd, MAX_CLIENT_COUNT) >= 0);
    while(1) {
        printf("Waiting for acceptions...\n");
        int tmp = accept(server_fd, (struct sockaddr *) &address, (socklen_t *)&addrlen);
        assert(tmp >= 0);
        printf("User connected\n");
        ++client_count;
        int * index = (int *)malloc(sizeof(int));
        *index = addElement(tmp);
        read(client_sock_fds[*index], client_names[*index],  NAME_SIZE);
        printf("Client name: %s\n", client_names[*index]);
        pthread_create(&write_thread, NULL, write_thread_fnc, (void *)index);
        pthread_detach(write_thread);
    }
    for (int i = 0; i < MAX_CLIENT_COUNT; ++i) {
        if (client_sock_fds[i] != -1) close(client_sock_fds[i]);
    }
    close(server_fd);
    return 0;
}