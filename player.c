#include <stdio.h>
#include <stdlib.h>
#define NUM_CARD 1024 
#define MAX_ID 13
#define BUFSZ 1024

struct Node {
   int id;
   struct Node *next;
} mem[NUM_CARD], *head, *tail;
int num_card;
int mcnt;
int count[MAX_ID+1], rmcnt[MAX_ID+1];

inline struct Node *getNode() { return &mem[mcnt++]; }
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
   head = getNode();
   head->id = -1;
   head->next = tail = NULL;

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
   struct Node *p = getNode();

   num_card++;
   count[id]++;

   p->id = id;
   p->next = NULL;

   if (tail == NULL)
      head->next = tail = p;
   else {
      tail->next = p;
      tail = tail->next;
   }
}

void remove_dup()
{
   struct Node *p = head;
   int i;

   for (i = 0; i <= MAX_ID; i++)
      rmcnt[i] = (count[i] / 2) * 2;

   while (p->next) {
      if (rmcnt[p->next->id] > 0) {
         num_card--;
         count[p->next->id]--;
         rmcnt[p->next->id]--;
         p->next = p->next->next;
      }
      else
         p = p->next;
   }
   tail = p;
}

void remove_card(int id)
{
   struct Node *p;

   for (p = head; p->next; p = p->next) {
      if (p->next->id == id) {
         num_card--;
         count[id]--;
         p->next = p->next->next;
         break;
      }
   }
   if (!p->next) tail = p;
}

int get_card_n(int n)
{
   struct Node *p = head->next;
   int i;

   for (i = 0; i < n; i++) p = p->next;

   return p->id;
}
