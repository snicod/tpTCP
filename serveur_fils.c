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

    // Création de la socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        return 2;
    }

    // Liaison de la socket au port donné
    struct sockaddr_in serv_addr;
    bzero((char*) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
    if (bind(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error binding socket to port");
        return 2;
    }

    // Mise en écoute de la socket
    listen(sockfd, 5);

    // Accepte une connexion entrante et gère la connexion avec un processus fils
    while (1) {
        // Accepte une connexion entrante
        struct sockaddr_in cli_addr;
        socklen_t clilen = sizeof(cli_addr);
        int newsockfd = accept(sockfd, (struct sockaddr*) &cli_addr, &clilen);
        if (newsockfd < 0) {
            perror("Error accepting connection");
            return 1;
        }
        print_client_info(cli_addr);

        // Créer un processus fils pour gérer la connexion
        pid_t pid = fork();
        if (pid == 0) {
            // Processus fils
            // Fermer la socket d'écoute dans le processus fils
            close(sockfd);

            // Lecture des données sur la socket et écriture sur la sortie standard
            char buffer[BUFSIZE];
            int n;
            while ((n = read(newsockfd, buffer, BUFSIZE)) > 0) {
                write(STDOUT_FILENO, buffer, n);
            }
            if (n < 0) {
                perror("Error reading from socket");
                return 3;
            }

            // Fermer la socket de la connexion dans le processus fils
            close(newsockfd);
            // Quitter le processus fils
            exit(0);
        }
        else if (pid > 0) {
            // Processus père
            // Fermer la socket de la connexion dans le processus père
            close(newsockfd);
        }
        else {
            perror("Error creating child process");
            return 4;
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

    printf("Connected to client %s : %d (PID %d)\n", client_ip, client_port, getpid());
}

