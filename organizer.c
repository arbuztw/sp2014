#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/select.h>
#define ERR_EXIT(s) { perror(s); exit(1); }
#define MAX_JUDGE 16
#define MAX_PLAYER 20
#define MAX_COMP 2000

typedef struct {
   int read_fd;
   int write_fd;
} Judge;

Judge judge[MAX_JUDGE];
int score[MAX_PLAYER], player_id[MAX_PLAYER];
int comp[MAX_COMP][4], comp_num;
void assign(int judgeid, int comp_id);
int get_loser(int fd);
void sortByScore(int n);


int main(int argc, char *argv[])
{
   int judge_num, player_num, ret_num;
   int fd1[2], fd2[2];
   int maxfd = 0, loser;
   int i, j, k, l;
   char judgeid[4];
   fd_set rset, cset;

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
         if (execlp("./judge", "judge", judgeid, (char*)0) < 0)
            ERR_EXIT("execlp");
      }
      close(fd1[0]);
      close(fd2[1]);
      judge[i].read_fd = fd2[0];
      judge[i].write_fd = fd1[1];
      if (fd2[0] > maxfd) maxfd = fd2[0];
   }

   comp[0][0] = comp[0][1] = comp[0][2] = comp[0][3] = 0;
   comp_num = 1;
   for (i = 1; i <= player_num; i++)
      for (j = i + 1; j <= player_num; j++)
         for (k = j + 1; k <= player_num; k++)
            for (l = k + 1; l <= player_num; l++)
            {
               comp[comp_num][0] = i;
               comp[comp_num][1] = j;
               comp[comp_num][2] = k;
               comp[comp_num++][3] = l;
            }
   ret_num = comp_num;

   FD_ZERO(&rset);
   for (i = 1; i <= judge_num; i++) FD_SET(judge[i].read_fd, &rset);

   for (i = 1; i <= judge_num; i++)
      assign(i, --comp_num);


   while (ret_num > 1) {
      memcpy(&cset, &rset, sizeof(rset));
      if (select(maxfd + 1, &cset, NULL, NULL, NULL) < 0)
         ERR_EXIT("select");
      for (i = 1; i <= judge_num; i++) {
         if (!FD_ISSET(judge[i].read_fd, &cset)) continue;
         loser = get_loser(judge[i].read_fd);
         score[loser]--;
         ret_num--;
         if (comp_num > 1)
            assign(i, --comp_num);
      }
   }

   for (i = 1; i <= judge_num; i++)
      assign(i, 0);


   for (i = 1; i <= player_num; i++) player_id[i] = i;
   sortByScore(player_num);

   for (i = 1; i < player_num; i++)
      fprintf(stderr, "%d ", player_id[i]);
   fprintf(stderr, "%d\n", player_id[i]);
   
   return 0;
}

void assign(int judgeid, int compid)
{
   char buf[PIPE_BUF];

   sprintf(buf, "%d %d %d %d\n", comp[compid][0], comp[compid][1], comp[compid][2], comp[compid][3]);
   write(judge[judgeid].write_fd, buf, strlen(buf));
}

int get_loser(int fd)
{
   char buf[1024];
   int i, val;

   read(fd, &buf, sizeof(buf));

   val = 0;
   for (i = 0; buf[i] != '\n'; i++)
      val = val * 10 + buf[i] - '0';
   return val;
}

void sortByScore(int n)
{
   int i, j, tmp;

   for (i = 1; i <= n; i++)
      for (j = i + 1; j <= n; j++)
         if (score[i] < score[j])
         {
            tmp = score[i];
            score[i] = score[j];
            score[j] = tmp;
            tmp = player_id[i];
            player_id[i] = player_id[j];
            player_id[j] = tmp;
         }
}
