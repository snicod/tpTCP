#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUFSIZE 1024

int main(int argc, char* argv[]) {
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent* server;
    char buffer[BUFSIZE];

    // Vérifier que nom de machine et numéro de port ont été passés en argument
    if (argc < 3) {
        fprintf(stderr, "Usage: %s hostname port\n", argv[0]);
        return 1;
    }

    // Récupérer le numéro de port
    portno = atoi(argv[2]);

    // Créer socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0); // SOCK_STREAM pour TCP et AF_INET pour IP
    if (sockfd < 0) {
        perror("Error opening socket");
        return 1;
    }

    // Récupérer adresse de la machine donnée en argument
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "Error, no such host\n");
        return 1;
    }

    // Remplir la structure sockaddr_in avec adresse et le numéro de port
    bzero((char*) &serv_addr, sizeof(serv_addr)); //mettre à 0 une zone mémoire
    serv_addr.sin_family = AF_INET;
    bcopy((char*) server->h_addr, (char*) &serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    // Se connecter à la socket sur adresse et port donnés
    if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error connecting");
        return 1;
    }

    // Lire l'entrée standard ligne par ligne
    while (fgets(buffer, BUFSIZE, stdin) != NULL) {
        // Envoyer la ligne lue à la socket
        n = write(sockfd, buffer, strlen(buffer));
        if (n < 0) {
            perror("Error writing to socket");
            return 1;
        }
    }

    // Fermer la socket
    close(sockfd);
    return 0;
}