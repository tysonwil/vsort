#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include "sort.h"

void
usage(char *prog) 
{
    fprintf(stderr, "usage: %s <-i file> [-q]\n", prog);
    exit(1);
}

int
main(int argc, char *argv[])
{
    // arguments
    char *inFile = "/no/such/file";

    // input params
    int c;
    int quiet = 0;
    opterr = 0;
    while ((c = getopt(argc, argv, "i:q")) != -1) {
	switch (c) {
	case 'i':
	    inFile = strdup(optarg);
	    break;
	case 'q':
	  quiet = 1;
	    break;
	default:
	    usage(argv[0]);
	}
    }

    // open and create output file
    int fd = open(inFile, O_RDONLY);
    if (fd < 0) {
	perror("open");
	exit(1);
    }

    rec_t r;
    while (1) {	
	int rc;
	rc = read(fd, &r.key, sizeof(r.key));
	if (rc == 0) // 0 indicates EOF
	    break;
	if (rc < 0) {
	    perror("read");
	    exit(1);
	}
	rc = read(fd, &r.size, sizeof(r.size));
	if (rc == 0) // 0 indicates EOF
	    break;
	if (rc < 0) {
	    perror("read");
	    exit(1);
	}
	rc = read(fd, r.record, RECDATASIZE(r));
	if (rc == 0) // 0 indicates EOF
	    break;
	if (rc < 0) {
	    perror("read");
	    exit(1);
	}
	printf("key:%9d:%4d rec:", r.key,r.size);
	if (!quiet) {
	  int j;	
	  for (j = 0; j < r.size; j++) 
	    printf("%3d ", r.record[j]);
	}
	printf("\n");
    }
    
    (void) close(fd);

    return 0;
}

