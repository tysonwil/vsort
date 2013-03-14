//
//  vsort.c
//
//
//  Created by Tyson Williams on 1/27/13.
//
//


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include "sort.h"
#include <sys/types.h>
#include <sys/stat.h>

void usage(char *prog)
{
    fprintf(stderr, "Usage: vsort -i inputfile -o outputfile\n");
    exit(1);
}

int compare_key(const void * v1, const void * v2)
{
    rec_t x = * (rec_t *) v1;
    rec_t y = * (rec_t *) v2;
    int i1 = x.key;
    int i2 = y.key;
    return ((i1 < i2) ? -1 : ((i1 > i2) ? 1 : 0));
}

int main (int argc, char *argv[])
{
    
    int count = 0;
    
    // arguments
    char *inFile = "/no/such/file";
    char *outFile = "/no/such/file";
    
	//program assumes a 4-byte key
	assert(sizeof(rec_t) == 1024);
    
	//input params
	int c;
	opterr = 0;
    if (argc != 5) usage(argv[0]);
	while ((c = getopt(argc, argv, "i:o:")) != -1) {
		switch (c) {
            case 'i':
                inFile = strdup(optarg);
                break;
            case 'o':
                outFile = strdup(optarg);
                break;
            default:
                usage(argv[0]);
		}
	}
    
    
    // open and create output file
    int input = open(inFile, O_RDONLY);
    if (input < 0) {
        fprintf(stderr, "Error: Cannot open file %s",inFile);
        exit(1);
    }
    int output = open(outFile, O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU);
    if (output < 0) {
        perror("open");
        exit(1);
    }
    
    rec_t r;
    while (1) {
        int rc;
        rc = read(input, &r.key, sizeof(r.key));
        if (rc == 0)
            break;
        if (rc < 0) {
            perror("read");
            exit(1);
        }
        rc = read(input, &r.size, sizeof(r.size));
        if (rc == 0) // 0 indicates EOF
            break;
        if (rc < 0) {
            perror("read");
            exit(1);
        }
        rc = read(input, r.record, RECDATASIZE(r));
        if (rc == 0) // 0 indicates EOF
            break;
        if (rc < 0) {
            perror("read");
            exit(1);
        }
        count++;
    }
    
    (void) close(output);
    

    int outputNext = open(outFile, O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU);
    if (outputNext < 0) {
        perror("open");
        exit(1);
    }
    
    int ponies = open(inFile, O_RDONLY);
    if (ponies < 0) {
        perror("open");
        exit(1);
    }
    
    rec_t r2;
    int z = 0;

    rec_t * array;
    array = malloc(sizeof(r2) * count);

    while (1) {
        int x;
        x = read(ponies, &r2.key, sizeof(r2.key));
        if (x == 0)
            break;
        if (x < 0) {
            perror("read");
            exit(1);
        }
        x = read(ponies, &r2.size, sizeof(r2.size));
        if (x == 0) // 0 indicates EOF
            break;
        if (x < 0) {
            perror("read");
            exit(1);
        }
        x = read(ponies, r2.record, RECDATASIZE(r2));
        if (x == 0) // 0 indicates EOF
            break;
        if (x < 0) {
            perror("read");
            exit(1);
        }
        array[z] = r2;
        z++;
    }
    
    /*
    int i;
    printf("Before sort:\n");
    for (i=0;i<count;i++) {
        rec_t a = array[i];
        printf("%d \n",a.key);
    }
    */

    qsort(array,count,sizeof(r2),compare_key);

    //printf("After sort:\n");
    int i;
     for (i=0;i<count;i++) {
        rec_t a = array[i];
        //printf("%d \n",a.key);
	write(outputNext, &a, RECSIZE(a));
    }
    
    (void) close(outputNext);
    
    return 0;
}
