#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFSIZE 4096

void print_client_info(struct sockaddr_in cli_addr);

int main(int argc, char *argv[]) {
    if (argc != 2) {
    fprintf(stderr, "Usage: %s port\n", argv[0]);
    return 1;
}
const int port = atoi(argv[1]);

// Création socket
int sockfd = socket(AF_INET, SOCK_STREAM, 0);
if (sockfd < 0) {
    perror("Error creating socket");
    return 2;
}

// Liaison socket au port donné
struct sockaddr_in serv_addr;
bzero((char*) &serv_addr, sizeof(serv_addr));
serv_addr.sin_family = AF_INET;
serv_addr.sin_addr.s_addr = INADDR_ANY;
serv_addr.sin_port = htons(port);
if (bind(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
    perror("Error binding socket to port");
    return 2;
}

// Mise en écoute socket
listen(sockfd, 5);

// Accepte connexion entrante et gère connexion petit-fils
while (1) {
    // Accepte une connexion 
    struct sockaddr_in cli_addr;
    socklen_t clilen = sizeof(cli_addr);
    int newsockfd = accept(sockfd, (struct sockaddr*) &cli_addr, &clilen);
    if (newsockfd < 0) {
        perror("Error accepting connection");
        return 1;
    }
    print_client_info(cli_addr);

    // Création fils pour gérer connexion
    pid_t child_pid = fork();
    if (child_pid == 0) {
        // Processus fils
        // Création petit-fils pour gérer connexion
        pid_t grandchild_pid = fork();
        if (grandchild_pid == 0) {
            // Processus petit-fils (chargé de la connexion)
            // Lecture des données sur socket et écriture sur sortie standard
            char buffer[BUFSIZE];
            int n;
            while ((n = read(newsockfd, buffer, BUFSIZE)) > 0) {
                write(STDOUT_FILENO, buffer, n);
            }
            if (n < 0) {
                perror("Error reading from socket");
                return 3;
            }

        close(newsockfd);
        exit(0);
        } else {
            // Processus fils (ne fait rien, se termine immédiatement)
            exit(0);
        }
    } else {
        // Processus père (ne fait rien de particulier)
    }
    }

    close(sockfd);

    return 0;
}

void print_client_info(struct sockaddr_in cli_addr) {
    char client_ip[INET_ADDRSTRLEN];
    int client_port;
    // Récupérer l'adresse IP du client
    inet_ntop(AF_INET, &(cli_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
    // Récupérer le numéro de port du client
    client_port = ntohs(cli_addr.sin_port);

    printf("Connected to client %s : %d\n", client_ip, client_port);

}
