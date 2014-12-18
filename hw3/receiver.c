#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUFSZ 16
#define err_exit(s) { perror(s); exit(1); }

FILE *flog;

void sig_int(int signo);

int main(int argc, char *argv[])
{
   if (argc != 2) {
      fprintf(stderr, "Usage: %s [test_data]\n", argv[0]);
      exit(1);
   }

   int fd[2], count[3] = {0};
   char buf[BUFSZ];

   pipe(fd);
   if (fork() == 0) {
      dup2(fd[1], STDOUT_FILENO);
      close(fd[0]);
      close(fd[1]);
      if (execlp("./sender", "sender", argv[1], (char*)0) < 0)
         err_exit("execlp");
   }
   close(fd[1]);

   flog = fopen("receiver_log", "w");

   signal(SIGINT, sig_int);

   while (read(fd[0], buf, BUFSZ) > 0) {
      if (!strcmp(buf, "receiver\n")) {
         fprintf(flog, "receive 0 %d\n", count[0]);
         usleep(1000);
         fprintf(flog, "finish 0 %d\n", count[0]++);
      }
   }

   fprintf(flog, "terminate\n");
   fclose(flog);

   return 0;
}

void sig_int(int signo)
{
   fprintf(flog, "terminate\n");   
   exit(0);
}
