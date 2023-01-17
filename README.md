# TP de manipulation de serveur TCP

Un seul type de client est créé mais plusieurs type de serveur sont à tester


server_phase1.c permet d'acceuillir une seule connexion à la fois

server_pere.c permet d'acceuillir plusieurs connexion géré par le processus père

server_fils.c permet d'acceuillir plusieurs connexion géré par le processus fils

server_pf.c permet d'acceuillir plusieurs connexions géré par le processus petit fils et permet d'éviter les processus zombies qui apparaissent si on utilise server_fils.c
