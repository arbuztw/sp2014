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


void init();
void sort(Player arr[], int n);

int main(int argc, char *argv[])
{
	int i, stat;
	int fd_rd, fd_wr[4];
	Player p[4];
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

	for (i = 0; i < 4; i++) {
		sprintf(fname, "judge%s_%c.FIFO", argv[1], 'A'+i);
		if (mkfifo(fname, 0664) < 0)
			ERR_EXIT("mkfifo");
	} 

	while (~scanf("%d %d %d %d", &p[0].id, &p[1].id, &p[2].id, &p[3].id)) {
		sort(p, 4);
		for (i = 0; i < 4; i++) {
			sprintf(pindex, "%c", 'A'+i);
			sprintf(key, "%d", rand() % 65536);
			if ((p[i].pid = fork()) == 0) {
				if (execlp("./player", "player", argv[1], pindex, key, (char*)0) < 0)
					ERR_EXIT("execl");
			}
		}

		//sleep(5);
		for (i = 0; i < 4; i++) {
			kill(p[i].pid, SIGTERM);
		}
		while (wait(&stat) > 0);
	}

	


	

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
