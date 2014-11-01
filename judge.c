#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#define SWAP(a,b) a^=b^=a^=b
#define ERR_EXIT(s) { perror(s); exit(1); }

typedef struct {
   int id;
   int num_card;
   int pipe_fd;
   int pid;
   int key;
} Player;

Player p[4];
int pile[53];
char child_fifo[4][16];

void init();
void sort(Player arr[], int n);
void shuffle(int arr[], int n);
void make_child_fifo();
void clean_child_fifo();

int main(int argc, char *argv[])
{
   int i, stat;
   int fd_rd;
   char fname[16], pindex[4], key[8];


   if (argc != 2) {
      fprintf(stderr, "Usage: %s [judge_id]\n", argv[0]);
      exit(1);
   }

   sprintf(fname, "judge%s.FIFO", argv[1]);

   if (mkfifo(fname, 0664) < 0)
      ERR_EXIT("mkfifo");

   if ((fd_rd = open(fname, O_RDONLY | O_NONBLOCK)) < 0)
      ERR_EXIT("open");
   unlink(fname);

   make_child_fifo(argv[1]);

   while (~scanf("%d %d %d %d", &p[0].id, &p[1].id, &p[2].id, &p[3].id)) {
      sort(p, 4);
      init();
      for (i = 0; i < 4; i++) {
         sprintf(pindex, "%c", 'A'+i);
         sprintf(key, "%d", p[i].key);
         if ((p[i].pid = fork()) == 0) {
            if (execlp("./player", "player", argv[1], pindex, key, (char*)0) < 0)
               ERR_EXIT("execl");
         }
         p[i].pipe_fd = open(child_fifo[i], O_WRONLY);
      }

      //sleep(5);
      for (i = 0; i < 4; i++) {
         kill(p[i].pid, SIGTERM);
      }
      while (wait(&stat) > 0);
   }

   clean_child_fifo();



   return 0;
}

void sort(Player arr[], int n)
{
   int i, j;
   Player tmp;
   for (i = 0; i < n; i++)
      for (j = i + 1; j < n; j++)
         if (arr[i].id > arr[j].id)
         {
            tmp = arr[i];
            arr[i] = arr[j];
            arr[j] = tmp;
         }
}

void init()
{
   int i, j;

   for (i = 0; i < 4; i++)
   {
      p[i].num_card = i > 0 ? 13 : 14;
      p[i].key = rand() % 65536;
   }
   
   pile[0] = 0;
   for (i = 1; i <= 4; i++)
      for (j = 1; j <= 13; j++)
         pile[i*j] = j;
   shuffle(pile, 53);
}

void shuffle(int arr[], int n)
{
   int i, j, tmp;

   for (i = 0; i < n; i++)
   {
      j = rand() % (i + 1);
      tmp = arr[j];
      arr[j] = arr[i];
      arr[i] = tmp;
   }
}

void make_child_fifo(char *judge)
{
   int i;

   for (i = 0; i < 4; i++) {
      sprintf(child_fifo[i], "judge%s_%c.FIFO", judge, 'A'+i); 
      if (mkfifo(child_fifo[i], 0666) < 0)
         ERR_EXIT("mkfifo");
   }
}

void clean_child_fifo()
{
   int i;

   for (i = 0; i < 4; i++) {
      if (unlink(child_fifo[i]) < 0)
         ERR_EXIT("unlink");
   }
}
