#include "lib.h"

int sock_fd = 0;
char client_name[NAME_SIZE] = {0};
char server_name[NAME_SIZE] = {0};
char send_buffer[BUFFER_SIZE] = {0};
char receve_buffer[BUFFER_SIZE] = {0};

void * read_thread_fnc(void * arg) {
    while (1) {
        if (read(sock_fd, receve_buffer, BUFFER_SIZE) <= 0) {
            printf("Server terminated.\n");
            kill(0, SIGKILL);
        };
        
        receve_buffer[strlen(receve_buffer) - 1] = '\0';
        printf("\r");
        printf("%s\n", receve_buffer);
        printf("%s: ", client_name);
        fflush(stdout);
        memset(receve_buffer, 0, BUFFER_SIZE);
    }
    return NULL;
} 

void * write_thread_fnc(void * arg) {
    while (1) {
        printf("%s: ", client_name);
        fgets(send_buffer, BUFFER_SIZE - 2, stdin);
        write(sock_fd, send_buffer, strlen(send_buffer));
        if (!strcmp(send_buffer, "exit\n")) {
            printf("Exiting...\n");
            exit(0);
        }
        memset(send_buffer, 0, BUFFER_SIZE);
    }
    return NULL;
}

int main(int argc, char * argv[]) {
    struct sockaddr_in serv_addr;
    pthread_t write_thread;
    pthread_t read_thread; 
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    assert(sock_fd >= 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[1]));
    assert(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) > 0);
    assert(connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) >= 0);
    printf("Enter your name: ");
    scanf("%s", client_name);
    write(sock_fd, client_name, strlen(client_name) + 1);
    getchar();;;;
    pthread_create(&write_thread, NULL, write_thread_fnc, NULL);
    pthread_create(&read_thread, NULL, read_thread_fnc, NULL);
    pthread_join(write_thread, NULL);
    pthread_join(read_thread, NULL);
    close(sock_fd);
    return 0;
}