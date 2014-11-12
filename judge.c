#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#define BUFSZ 1024
#define SWAP(a,b) a^=b^=a^=b
#define ERR_EXIT(s) { perror(s); exit(1); }

typedef struct {
   int id;
   int num_card;
   int pid;
   int key;
   FILE *pipe;
} Player;

Player p[4];
int pile[53];
char child_fifo[4][16];
FILE *fin;

void init();
void sort(Player arr[], int n);
void shuffle(int arr[], int n);
void make_child_fifo();
void clean_child_fifo();
void deal_card();
int read_from_pipe(int *num);

int main(int argc, char *argv[])
{
   int i, stat;
   int cnt, cur, nxt, num;
   char fname[16], pindex[4], key[8];


   if (argc != 2) {
      fprintf(stderr, "Usage: %s [judge_id]\n", argv[0]);
      exit(1);
   }

   sprintf(fname, "judge%s.FIFO", argv[1]);

   if (mkfifo(fname, 0664) < 0)
      ERR_EXIT("mkfifo");

   make_child_fifo(argv[1]);

   scanf("%d %d %d %d", &p[0].id, &p[1].id, &p[2].id, &p[3].id);
   while (p[0].id || p[1].id || p[2].id || p[3].id) {
      sort(p, 4);
      init();
      for (i = 0; i < 4; i++) {
         sprintf(pindex, "%c", 'A'+i);
         sprintf(key, "%d", p[i].key);
         if ((p[i].pid = fork()) == 0) {
            if (execlp("./player", "player", argv[1], pindex, key, (char*)0) < 0)
               ERR_EXIT("execl");
         }
         p[i].pipe = fopen(child_fifo[i], "w");
      }

      fin = fopen(fname, "r");

      deal_card();

      cnt = 0;
      for (i = 0; i < 4; i++)
         if (p[i].num_card > 0)
            cnt++;

      cur = 0;
      while (cnt > 1) {
         while (p[cur].num_card == 0) cur = (cur + 1) % 4;
         nxt = (cur + 1) % 4;
         while (p[nxt].num_card == 0) nxt = (nxt + 1) % 4;

         fprintf(p[cur].pipe, "< %d\n", p[nxt].num_card);
         fflush(p[cur].pipe);

         read_from_pipe(&num);
         fprintf(p[nxt].pipe, "> %d\n", num);
         fflush(p[nxt].pipe);
         p[nxt].num_card--;

         read_from_pipe(&num);
         fprintf(p[cur].pipe, "%d\n", num);
         fflush(p[cur].pipe);

         read_from_pipe(&num);
         if (num) p[cur].num_card--;
         else p[cur].num_card++;

         if (p[cur].num_card == 0) cnt--;
         if (p[nxt].num_card == 0) cnt--;

         cur = nxt;
      }

      for (i = 0; i < 4; i++)
         fclose(p[i].pipe);
      fclose(fin);
      while (wait(&stat) > 0);

      for (i = 0; i < 4; i++)
         if (p[i].num_card > 0) {
            printf("%d\n", p[i].id);
            fflush(stdout);
         }

      scanf("%d %d %d %d", &p[0].id, &p[1].id, &p[2].id, &p[3].id);
   }

   clean_child_fifo();
   unlink(fname);



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
   for (i = 0; i < 4; i++)
      for (j = 1; j <= 13; j++)
         pile[i*13+j] = j;
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

void deal_card()
{
   int i, j;
   int pidx, num;
   for (i = 0; i < 4; i++) {
      if (i == 0) fprintf(p[i].pipe, "%d ", pile[0]);
      for (j = 1; j < 13; j++)
         fprintf(p[i].pipe, "%d ", pile[i*13+j]);
      fprintf(p[i].pipe, "%d\n", pile[i*13+j]);
      fflush(p[i].pipe);
   }

   for (i = 0; i < 4; i++) {
      pidx = read_from_pipe(&num);
      p[pidx].num_card = num;
   }
}

int read_from_pipe(int *num)
{
   int i, pidx, key;
   char buf[BUFSZ];

   fgets(buf, BUFSZ, fin);

   pidx = buf[0] - 'A';
   key = 0;
   for (i = 2; buf[i] != ' '; i++)
      key = key * 10 + buf[i] - '0';
   if (key != p[pidx].key) {
      fprintf(stderr, "Someone cheated!\n");
      exit(1);
   }
   *num = 0;
   for (i++; buf[i] != '\n'; i++)
      *num = *num * 10 + buf[i] - '0';
   return pidx;
}
