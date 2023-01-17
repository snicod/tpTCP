#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>

#define BUFSIZE 1024

// Fonction pour afficher les informations du client (adresse et numéro de port)
void print_client_info(struct sockaddr_in cli_addr) {
    char client_ip[INET_ADDRSTRLEN];
    int client_port;

    // Récupérer l'adresse IP du client
    inet_ntop(AF_INET, &(cli_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
    // Récupérer le numéro de port du client
    client_port = ntohs(cli_addr.sin_port);

    printf("Connected to client %s : %d\n", client_ip, client_port);
}

int main(int argc, char* argv[]) {
    int sockfd, portno, clilen;
    struct sockaddr_in serv_addr, cli_addr;
    char buffer[BUFSIZE];
    int n;
    pid_t pid;

    // Vérifier que le numéro de port a été passé en argument
    if (argc < 2) {
        fprintf(stderr, "Usage: %s port\n", argv[0]);
        return 1;
    }

    // Créer la socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error opening socket");
        return 1;
    }

    // Remplir la structure sockaddr_in avec le numéro de port donné en argument
    bzero((char*) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    // Lier la socket au numéro de port donné
    if (bind(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error binding socket to port");
        return 1;
    }
    // Mettre la socket en écoute
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    while (1) {
        // Accepte une connexion entrante
        int newsockfd = accept(sockfd, (struct sockaddr*) &cli_addr, &clilen);
        if (newsockfd < 0) {
            perror("Error accepting connection");
            return 1;
        }
        print_client_info(cli_addr);

        // TODO : gère la connexion avec le client ici
        // Lecture de données envoyées par le client
        char buffer[256];
        int n;
        while ((n = read(newsockfd, buffer, 255)) > 0) {
            // Ajoute un caractère de fin de chaîne à la fin des données lues
            buffer[n] = '\0';
            // Affiche les données lues
            printf("Received from client: %s\n", buffer);

            // Envoie une réponse au client
            n = write(newsockfd, "ACK", 3);
            if (n < 0) {
                perror("Error writing to socket");
                return 1;
            }
        }
        if (n < 0) {
            perror("Error reading from socket");
            return 1;
        }
    }


}