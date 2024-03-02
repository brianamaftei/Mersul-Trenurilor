#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include "../utils/Command.cpp"
#include <bits/stdc++.h>
#include <queue>
#include <ctime>

using namespace std;

/* portul folosit */
#define PORT 2908
#define MAX_OPTIUNI_SIZE 1000

/* codul de eroare returnat de anumite apeluri */
extern int errno;

typedef struct thData
{
  int idThread; // id-ul thread-ului tinut in evidenta de acest program
  int cl;       // descriptorul intors de accept
} thData;

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

queue<Client> commandQueue;

const char *handleCommand(Client current, Invoker invoker);
static void *treat(void *); /* functia executata de fiecare thread ce realizeaza comunicarea cu clientii */
void raspunde(void *);
static void *treatQueue(void *);
void takeFromQueue();
void addToQueue(Client client);

XmlHandle xml;

int main()
{
  struct sockaddr_in server; // structura folosita de server
  struct sockaddr_in from;
  int nr; // mesajul primit de trimis la client
  int sd; // descriptorul de socket
  int pid;
  pthread_t th[100]; // Identificatorii thread-urilor care se vor crea
  int i = 0;

  /* crearea unui socket */
  if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror("[server]Eroare la socket().\n");
    return errno;
  }
  /* utilizarea optiunii SO_REUSEADDR */
  int on = 1;
  setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

  /* pregatirea structurilor de date */
  bzero(&server, sizeof(server));
  bzero(&from, sizeof(from));

  /* umplem structura folosita de server */
  /* stabilirea familiei de socket-uri */
  server.sin_family = AF_INET;
  /* acceptam orice adresa */
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  /* utilizam un port utilizator */
  server.sin_port = htons(PORT);

  /* atasam socketul */
  if (bind(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
  {
    perror("[server]Eroare la bind().\n");
    return errno;
  }

  /* punem serverul sa asculte daca vin clienti sa se conecteze */
  if (listen(sd, 2) == -1)
  {
    perror("[server]Eroare la listen().\n");
    return errno;
  }

  pthread_t deathlessThread;
  pthread_create(&deathlessThread, NULL, &treatQueue, NULL);

  printf("[server]Asteptam la portul %d...\n", PORT);
  /* servim in mod concurent clientii...folosind thread-uri */

  while (1)
  {
    int client;
    thData *td; // parametru functia executata de thread
    unsigned int length = sizeof(from);

    fflush(stdout);

    // client= malloc(sizeof(int));
    /* acceptam un client (stare blocanta pina la realizarea conexiunii) */
    if ((client = accept(sd, (struct sockaddr *)&from, &length)) < 0)
    {
      perror("[server]Eroare la accept().\n");
      continue;
    }

    /* s-a realizat conexiunea, se astepta mesajul */

    // int idThread; //id-ul threadului
    // int cl; //descriptorul intors de accept

    td = (struct thData *)malloc(sizeof(struct thData));
    td->idThread = i++;
    td->cl = client;

    pthread_create(&th[i], NULL, &treat, td);
    printf("[server]S-a conectat un client la server\n");
  }

  xml.endXml();
};

static void *treat(void *arg)
{
  struct thData tdL;
  tdL = *((struct thData *)arg);
  fflush(stdout);
  pthread_detach(pthread_self());
  raspunde((struct thData *)arg);
  /* am terminat cu acest client, inchidem conexiunea */
  return (NULL);
};

const char *getRequirement(int val)
{
  char *c = new char[100];
  if (val == 1)
    strcpy(c, "Introduceti <ID tren>:\n");
  else if (val == 2)
    strcpy(c, "Introduceti <statia de sosire>:\n");
  else if (val == 3)
    strcpy(c, "Introduceti <statia de plecare>:\n");
  else if (val == 4)
    strcpy(c, "Introduceti <ID tren> <statie>  <minute>  <intarziere/inainte>:\n");
  else
    strcpy(c, "Nimic\n");
  c[strlen(c)] = '\0';
  return c;
}

void raspunde(void *arg)
{
  struct thData tdL;
  tdL = *((struct thData *)arg);

  char optiuni[MAX_OPTIUNI_SIZE] = "Introduceti numarul actiunii pe care o doriti:\n";
  strcat(optiuni, "1 - Mersul trenurilor in aceasta zi\n");
  strcat(optiuni, "2 - Sosiri in urmatoarea ora\n");
  strcat(optiuni, "3 - Plecari in urmatoarea ora\n");
  strcat(optiuni, "4 - Sugerati o modifcare\n");
  strcat(optiuni, "5 - Iesire din program\n");
  int lg = strlen(optiuni);
  optiuni[lg] = '\0';

  if (write(tdL.cl, &lg, sizeof(int)) <= 0)
  {
    printf("[Thread %d] ", tdL.idThread);
    printf("[Thread]Eroare la write() catre client.\n");
  }

  if (write(tdL.cl, optiuni, lg) <= 0)
  {
    printf("[Thread %d] ", tdL.idThread);
    printf("[Thread]Eroare la write() catre client.\n");
  }

  int finished = 0;
  int valoareComanda;

  while (!finished)
  {
    if (read(tdL.cl, &valoareComanda, sizeof(int)) <= 0)
    {
      printf("[Thread %d] ", tdL.idThread);
      printf("[Thread]Eroare la write() catre client.\n");
      break;
    }

    if (valoareComanda != 5)
    {
      char cerinta[100];
      const char *p = getRequirement(valoareComanda);
      strcpy(cerinta, p);

      cerinta[strlen(cerinta)] = '\0';
      int lg5 = strlen(cerinta);

      if (write(tdL.cl, &lg5, sizeof(int)) <= 0)
      {
        printf("[Thread %d] ", tdL.idThread);
        printf("[Thread]Eroare la write() catre client.\n");
        break;
      }

      if (write(tdL.cl, cerinta, lg5) <= 0)
      {
        printf("[Thread %d] ", tdL.idThread);
        printf("[Thread]Eroare la write() catre client.\n");
        break;
      }

      int lg8;
      char rezolvare[100] = "";

      if (read(tdL.cl, &lg8, sizeof(int)) <= 0)
      {
        printf("[Thread %d] ", tdL.idThread);
        printf("[Thread]Eroare la write() catre client.\n");
        break;
      }

      if (read(tdL.cl, rezolvare, lg8) <= 0)
      {
        printf("[Thread %d] ", tdL.idThread);
        printf("[Thread]Eroare la write() catre client.\n");
        break;
      }
      rezolvare[lg8] = '\0';

      Client client(tdL.cl, valoareComanda, rezolvare);
      addToQueue(client);
    }
    else
    {
      Invoker invoker;
      char message[1000];
      strcpy(message, handleCommand(Client(tdL.cl, valoareComanda, ""), invoker));
      int lg1 = strlen(message);
      message[lg1] = '\0';

      if (write(tdL.cl, &lg1, sizeof(int)) <= 0)
      {
        printf("[Thread %d] ", tdL.cl);
        printf("[Thread]Eroare la write() catre client.\n");
        break;
      }

      if (write(tdL.cl, message, lg1) <= 0)
      {
        printf("[Thread %d] ", tdL.cl);
        printf("[Thread]Eroare la write() catre client.\n");
        break;
      }

      break;
    }
  }
  close(tdL.cl);
}

static void *treatQueue(void *arg)
{
  takeFromQueue();
  return (NULL);
}

void takeFromQueue()
{
  pthread_t th1;
  int i = 0;
  int finished = 0;
  Client current(0, 0, "");
  while (!finished)
  {
    int full = 0;

    pthread_mutex_lock(&m);
    if (!commandQueue.empty())
    {
      current = commandQueue.front();
      commandQueue.pop();
      full = 1;
      // sleep(1);
    }
    pthread_mutex_unlock(&m);

    if (full == 1)
    {
      Invoker invoker;
      char message[1000];
      strcpy(message, handleCommand(current, invoker));
      int lg1 = strlen(message);
      message[lg1] = '\0';
      if (write(current.getCl(), &lg1, sizeof(int)) <= 0)
      {
        printf("[Thread %d] ", current.getCl());
        printf("[Thread]Eroare la write() catre client.\n");
      }

      if (write(current.getCl(), message, lg1) <= 0)
      {
        printf("[Thread %d] ", current.getCl());
        printf("[Thread]Eroare la write() catre client.\n");
      }
    }
  }
}

void addToQueue(Client client)
{
  pthread_mutex_lock(&m);
  commandQueue.push(client);
  pthread_mutex_unlock(&m);
}

const char *handleCommand(Client current, Invoker invoker)
{
  int valoare = current.getCommand();

  BaseCommand *result = nullptr;
  printf("[server]Se executa comanda %d\n", valoare);

  if (valoare == 1)
  {
    result = new InformationCommand(current, xml);
  }
  else if (valoare == 2)
  {
    result = new ArrivalsCommand(current, xml);
  }
  else if (valoare == 3)
  {
    result = new DeparturesCommand(current, xml);
  }
  else if (valoare == 4)
  {
    result = new ModifyCommand(current, xml);
  }
  else
  {
    result = new QuitCommand();
  }

  const char *resultMessage = invoker.executeCommand(result);
  delete result;

  return resultMessage;
}
