#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <regex>
#include <signal.h>
using namespace std;

/* codul de eroare returnat de anumite apeluri */
extern int errno;

/* portul de conectare la server*/
int port;

int main(int argc, char *argv[])
{
    int sd;                    // descriptorul de socket
    struct sockaddr_in server; // structura folosita pentru conectare
    char msg[100];             // mesajul trimis

    /* exista toate argumentele in linia de comanda? */
    if (argc != 3)
    {
        printf("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
        return -1;
    }

    /* stabilim portul */
    port = atoi(argv[2]);

    /* cream socketul */
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Eroare la socket().\n");
        return errno;
    }

    /* umplem structura folosita pentru realizarea conexiunii cu serverul */
    /* familia socket-ului */
    server.sin_family = AF_INET;
    /* adresa IP a serverului */
    server.sin_addr.s_addr = inet_addr(argv[1]);
    /* portul de conectare */
    server.sin_port = htons(port);

    /* ne conectam la server */
    if (connect(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
    {
        perror("[client]Eroare la connect().\n");
        return errno;
    }

    // if (signal(SIGINT, SIG_IGN) == SIG_ERR)
    // {
    //     perror("Eroare la setarea semnalului");
    //     return 1;
    // }

    int lg;
    char optiuni[1000];

    if (read(sd, &lg, sizeof(int)) < 0)
    {
        perror("[client]Eroare la read() de la server.\n");
        return errno;
    }

    if (read(sd, optiuni, lg) < 0)
    {
        perror("[client]Eroare la read() de la server.\n");
        return errno;
    }
    optiuni[strlen(optiuni)] = '\0';

    int finished = 0;
    while (!finished)
    {
        printf("%s", optiuni);
        fflush(stdout);

        int comandaValida = 0;
        char comanda[100];
        int valoareComanda = 5;

        while (!comandaValida)
        {
            fgets(comanda, 101, stdin);
            comanda[strlen(comanda) - 1] = '\0';
            if (strlen(comanda) == 1 && comanda[0] >= '1' && comanda[0] <= '5')
            {
                valoareComanda = atoi(comanda);
                comandaValida = 1;
            }
            else
            {
                printf("Valoare invalida. Introduceti o valoare din lista\n");
                fflush(stdout);
            }
        }

        if (write(sd, &valoareComanda, sizeof(int)) <= 0)
        {
            perror("[client]Eroare la write() valoareComanda spre server.\n");
            return errno;
        }

        if (valoareComanda == 5)
        {
            finished = 23;
        }
        else
        {
            int lg5;
            char cerinta[100];

            if (read(sd, &lg5, sizeof(int)) < 0)
            {
                perror("[client]Eroare la read() de la server.\n");
                return errno;
            }

            if (read(sd, cerinta, lg5) < 0)
            {
                perror("[client]Eroare la read() de la server.\n");
                return errno;
            }
            cerinta[lg5] = '\0';

            printf("%s", cerinta);
            fflush(stdout);

            int cerintaValida = 0;
            char rezolvare[100] = "";

            while (!cerintaValida)
            {
                fgets(rezolvare, 100, stdin);
                rezolvare[strlen(rezolvare) - 1] = '\0';

                regex words("[\\w-]+");
                cregex_iterator start(rezolvare, rezolvare + strlen(rezolvare), words);
                cregex_iterator end;

                int nrWords = distance(start, end);

                if ((valoareComanda >= 1 && valoareComanda <= 3 && nrWords == 1) || (valoareComanda == 4 && nrWords == 4))
                {
                    cerintaValida = 1;
                }
                else
                {
                    printf("Numar incorect de parametrii. Introduceti din nou:\n");
                    fflush(stdout);
                }
            }

            int lg8 = strlen(rezolvare);
            if (write(sd, &lg8, sizeof(int)) < 0)
            {
                perror("[client]Eroare la read() de la server.\n");
                return errno;
            }

            if (write(sd, rezolvare, lg8) < 0)
            {
                perror("[client]Eroare la read() de la server.\n");
                return errno;
            }
        }

        char mesaj[1000];
        int lg1;

        if (read(sd, &lg1, sizeof(int)) < 0)
        {
            perror("[client]Eroare la read() de la server.\n");
            return errno;
        }

        if (read(sd, mesaj, lg1) < 0)
        {
            perror("[client]Eroare la read() de la server.\n");
            return errno;
        }

        mesaj[lg1] = '\0';

        printf("%s", mesaj);
        fflush(stdout);
    }

    close(sd);
}
