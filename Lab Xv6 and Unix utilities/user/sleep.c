#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
int main(int argc, char *argv[]) //参数个数及内容
{
  int time;
  if (argc != 2) 
  {
    fprintf(2, "There's no arg.");
    exit(1);
  }
  time = atoi(argv[1]);//转换为int型
  sleep(time);
  exit(0);
}