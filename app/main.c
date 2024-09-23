#include <stdio.h>
#include <unistd.h> 
#include "../src/lab.h" 

void print_version()
{
  printf("Shell version: %d.%d\n", lab_VERSION_MAJOR, lab_VERSION_MINOR);
}

int main(int argc, char *argv[])
{
  int opt;

  // Parse command-line arguments
  while ((opt = getopt(argc, argv, "v")) != -1)
  {
    switch (opt)
    {
    case 'v':
      print_version();
      return 0;
    default:
      printf("Usage: %s [-v]\n", argv[0]);
      return 1;
    }
  }

  printf("hello world\n");
  return 0;
}
