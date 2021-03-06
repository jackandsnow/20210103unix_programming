/*************************************************************************
	> File Name: 1_getopt.c
	> Author: wangxue
	> Mail: 
	> Created Time: Mon Jan  4 19:47:42 2021
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]){
    int opt;
    while ( (opt = getopt(argc, argv, "alm:o::")) != -1 ) {
        switch (opt) {
            case 'a':
                printf("a here, optopt = %c, optarg=%s\n", optopt, optarg);
                break;
            case 'l':
                printf("l here, optopt=%c, optarg=%s\n", optopt, optarg);
                break;
            case 'm':
                printf("msg = %s\n", optarg);
                break;
            case 'o':
                printf("opt = %s\n", optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s -al\n", argv[0]);
                //exit(1);
        }
    }
    printf("optind=%d, opterr=%d, optopt=%c, argv[optind]=%s, argc %d\n", optind, opterr, optopt, argv[optind], argc);
}
