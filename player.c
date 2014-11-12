#include <stdio.h>
#include <stdlib.h>
#define NUM_CARD 16 
#define MAX_ID 13
#define BUFSZ 1024

int num_card, cards[16];
int mcnt;
int count[MAX_ID+1], rmcnt[MAX_ID+1];

void insert_card(int id);
void remove_dup();
void remove_card(int id);
int get_card_n(int n);

int main(int argc, char *argv[])
{
   int i, id, num, m;
   char type[4], fname[16];
   FILE *fin, *fout;
   
   sprintf(fname, "judge%s_%s.FIFO", argv[1], argv[2]);
   fin = fopen(fname, "r");
   sprintf(fname, "judge%s.FIFO", argv[1]);
   fout = fopen(fname, "w");

   m = (argv[2][0] == 'A') ? 14 : 13;
   num_card = 0;

   for (i = 0; i < m; i++) {
      fscanf(fin, "%d", &id);
      insert_card(id);
   }

   remove_dup();
   fprintf(fout, "%s %s %d\n", argv[2], argv[3], num_card);
   fflush(fout);

   while (~fscanf(fin, "%s %d", type, &num)) {
      if (type[0] == '<') {
         fprintf(fout, "%s %s %d\n", argv[2], argv[3], rand() % num);
         fflush(fout);
         fscanf(fin, "%d", &num);
         if (count[num] > 0) {
            remove_card(num);
            fprintf(fout, "%s %s 1\n", argv[2], argv[3]);
         }
         else {
            insert_card(num);
            fprintf(fout, "%s %s 0\n", argv[2], argv[3]);
         }
      }
      else {
         num = get_card_n(num);
         remove_card(num);
         fprintf(fout, "%s %s %d\n", argv[2], argv[3], num);
      }
      fflush(fout);
   }

   fclose(fin);
   fclose(fout);

   return 0;
}

void insert_card(int id)
{
   count[id]++;
   cards[num_card++] = id;
}

void remove_dup()
{
   int i, j, tmp = num_card;

   for (i = 0; i <= MAX_ID; i++)
      rmcnt[i] = (count[i] / 2) * 2;

   for (i = 0; i < tmp; i++) {
      if (rmcnt[cards[i]] > 0) {
         num_card--;
         count[cards[i]]--;
         rmcnt[cards[i]]--;
         cards[i] = -1;
      }
   }

   for (i = j = 0; i < tmp; i++) {
      if (cards[i] != -1) {
         cards[j++] = cards[i];
      }
   }
}

void remove_card(int id)
{
   int i;
   
   for (i = 0; i < num_card; i++) {
      if (cards[i] == id) {
         num_card--;
         while (i < num_card) {
            cards[i] = cards[i+1];
            i++;
         }
         break;
      }
   }
}

int get_card_n(int n)
{
   return cards[n];
}
