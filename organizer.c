#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#define ERR_EXIT(s) { perror(s); exit(1); }
#define MAX_JUDGE 16
#define MAX_PLAYER 20

typedef struct {
   int read_fd;
   int write_fd;
} Judge;

Judge judge[MAX_JUDGE];
int score[MAX_PLAYER], player_id[MAX_PLAYER];

int main(int argc, char *argv[])
{
   int judge_num, player_num;
   int fd1[2], fd2[2];
   int i, j, k;
   char judgeid[4], buf[PIPE_BUF];

   if (argc != 3) {
      fprintf(stderr, "Usage: %s [judge_num] [player_num]\n", argv[0]);
      exit(0);
   }

   judge_num = atoi(argv[1]);
   player_num = atoi(argv[2]);

   for (i = 1; i <= judge_num; i++) {
      pipe(fd1);
      pipe(fd2);
      if (fork() == 0) {
         dup2(fd1[0], STDIN_FILENO);
         dup2(fd2[1], STDOUT_FILENO);
         close(fd1[0]);
         close(fd1[1]);
         close(fd2[0]);
         close(fd2[1]);
         sprintf(judgeid, "%d", i);
         fprintf(stderr, "judge %s\n", judgeid);
         if (execlp("./judge", "judge", judgeid, (char*)0) < 0)
            ERR_EXIT("execlp");
      }
      close(fd1[0]);
      close(fd2[1]);
      judge[i].read_fd = fd2[0];
      judge[i].write_fd = fd1[1];
   }


   //while (1);
   
   return 0;
}
