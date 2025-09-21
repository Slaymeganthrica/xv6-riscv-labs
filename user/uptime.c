#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]){
  int ticks = uptime();
  fprintf(1, "uptime: %d ticks\n", ticks);

  exit(0);
}
