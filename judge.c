#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#define SWAP(a,b) a^=b^=a^=b
#define ERR_EXIT(s) { perror(s); exit(1); }


void sort(int arr[], int n);

int main(int argc, char *argv[])
{
	int i, stat;
	int fd_rd, fd_wr[4];
	int player[4], pid[4];
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

	while (~scanf("%d %d %d %d", &player[0], &player[1], &player[2], &player[3])) {
		sort(player, 4);
		for (i = 0; i < 4; i++) {
			sprintf(pindex, "%c", 'A'+i);
			sprintf(key, "%d", rand() % 65536);
			if ((pid[i] = fork()) == 0) {
				if (execlp("./player", "player", argv[1], pindex, key, (char*)0) < 0)
					ERR_EXIT("execl");
			}
		}

		//sleep(5);
		for (i = 0; i < 4; i++) {
			kill(pid[i], SIGTERM);
		}
		while (wait(&stat) > 0);
	}

	


	

	return 0;
}

void sort(int arr[], int n)
{
	int i, j, tmp;
	for (i = 0; i < n; i++)
		for (j = i + 1; j < n; j++)
			if (arr[i] > arr[j])
			{
				tmp = arr[i];
				arr[i] = arr[j];
				arr[j] = tmp;
			}
}
