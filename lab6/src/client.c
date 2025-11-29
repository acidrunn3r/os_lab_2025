#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>

#include <multmodulo.h>
struct Server {
  char ip[255];
  int port;
};

struct ThreadData {
  struct Server server;
  uint64_t begin;
  uint64_t end;
  uint64_t mod;
  uint64_t result;
};


bool ConvertStringToUI64(const char *str, uint64_t *val) {
  char *end = NULL;
  errno = 0;
  unsigned long long i = strtoull(str, &end, 10);
  if (errno == ERANGE) {
    fprintf(stderr, "Out of uint64_t range: %s\n", str);
    return false;
  }
  if (errno != 0)
    return false;
  *val = i;
  return true;
}

void *run_server(void *args) {
  struct ThreadData *data = (struct ThreadData *)args;
  struct hostent *hostname = gethostbyname(data->server.ip);
  if (hostname == NULL) {
    fprintf(stderr, "gethostbyname failed with %s\n", data->server.ip);
    pthread_exit(NULL);
  }

  struct sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_port = htons(data->server.port);
  server.sin_addr.s_addr = *((unsigned long *)hostname->h_addr);

  int sck = socket(AF_INET, SOCK_STREAM, 0);
  if (sck < 0) {
    fprintf(stderr, "Socket creation failed!\n");
    pthread_exit(NULL);
  }

  if (connect(sck, (struct sockaddr *)&server, sizeof(server)) < 0) {
    fprintf(stderr, "Connection failed to %s:%d\n", data->server.ip,
            data->server.port);
    pthread_exit(NULL);
  }

  char task[sizeof(uint64_t) * 3];
  memcpy(task, &data->begin, sizeof(uint64_t));
  memcpy(task + sizeof(uint64_t), &data->end, sizeof(uint64_t));
  memcpy(task + 2 * sizeof(uint64_t), &data->mod, sizeof(uint64_t));

  if (send(sck, task, sizeof(task), 0) < 0) {
    fprintf(stderr, "Send failed\n");
    close(sck);
    pthread_exit(NULL);
  }

  char response[sizeof(uint64_t)];
  if (recv(sck, response, sizeof(response), 0) < 0) {
    fprintf(stderr, "Receive failed\n");
    close(sck);
    pthread_exit(NULL);
  }

  memcpy(&data->result, response, sizeof(uint64_t));
  close(sck);
  pthread_exit(NULL);
}

int main(int argc, char **argv) {
  uint64_t k = -1;
  uint64_t mod = -1;
  char servers[255] = {'\0'};

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"k", required_argument, 0, 0},
                                      {"mod", required_argument, 0, 0},
                                      {"servers", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "", options, &option_index);

    if (c == -1)
      break;

    switch (c) {
    case 0: {
      switch (option_index) {
      case 0:
        ConvertStringToUI64(optarg, &k);
        break;
      case 1:
        ConvertStringToUI64(optarg, &mod);
        break;
      case 2:
        strncpy(servers, optarg, sizeof(servers) - 1);
        break;
      default:
        printf("Index %d is out of options\n", option_index);
      }
    } break;

    case '?':
      printf("Arguments error\n");
      break;
    default:
      fprintf(stderr, "getopt returned character code 0%o?\n", c);
    }
  }

  if (k == -1 || mod == -1 || !strlen(servers)) {
    fprintf(stderr, "Using: %s --k 1000 --mod 5 --servers /path/to/file\n",
            argv[0]);
    return 1;
  }

  FILE *file = fopen(servers, "r");
  if (!file) {
    perror("fopen");
    return 1;
  }

  struct Server *to = malloc(sizeof(struct Server) * 128);
  unsigned int servers_num = 0;
  while (fscanf(file, "%254[^:]:%d\n", to[servers_num].ip, &to[servers_num].port) == 2)
    servers_num++;
  fclose(file);

  if (servers_num == 0) {
    fprintf(stderr, "No servers found\n");
    free(to);
    return 1;
  }

  pthread_t threads[servers_num];
  struct ThreadData thread_data[servers_num];

  uint64_t chunk = k / servers_num;
  uint64_t rem = k % servers_num;
  uint64_t cur = 1;

  for (int i = 0; i < servers_num; i++) {
    thread_data[i].server = to[i];
    thread_data[i].begin = cur;
    thread_data[i].end = cur + chunk - 1;
    if (i < rem)
      thread_data[i].end++;
    cur = thread_data[i].end + 1;
    thread_data[i].mod = mod;
    thread_data[i].result = 1;

    pthread_create(&threads[i], NULL, run_server, &thread_data[i]);
  }

  uint64_t total = 1;
  for (int i = 0; i < servers_num; i++) {
    pthread_join(threads[i], NULL);
    total = MultModulo(total, thread_data[i].result, mod);
  }

  printf("Final result: %llu! mod %llu = %llu\n", k, mod, total);

  free(to);
  return 0;
}