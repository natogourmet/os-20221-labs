#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int main() {

  FILE *fp;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  fp = fopen("input.txt", "r");
  if (fp == NULL)
    exit(EXIT_FAILURE);

  while ((read = getline(&line, &len, fp)) != -1)
  {
    printf("Retrieved line of length %zu:\n", read);
    printf("%s", line);
  }

  fclose(fp);
  if (line)
    free(line);
  exit(EXIT_SUCCESS);

  // printf("%s\n", file_contents);


  return 0;

  // printf("El valor de x ahora es: %d\n", x);
  // printf("La direccion de x es: %p\n",(void *) &x);
  // printf("El apuntador p apunta a: %p \n", (void *) p);
}
