/* This program removes specified (or current) directory/file from Linux filesystem cache */
/* Idea from Tobias Oetiker http://insights.oetiker.ch/linux/fadvise/ */
#define _GNU_SOURCE
#define _XOPEN_SOURCE 600

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fts.h>
#include <fcntl.h>

int main(int argc, char ** argv) {
    FTS * tree;
    FTSENT * node;
    
    char **paths = malloc(argc*sizeof(char*));
    memcpy(paths,argv+1,(argc-1)*sizeof(char*));
    paths[argc]=NULL;
    if (argc<2) {   //paths[0]=getcwd(NULL,0);
        fprintf(stderr, "usage: %s <directory>", argv[0]);
        fprintf(stderr, " walks directory recursively and removes all files found from the cache\n");
        exit(1);
    }
    tree=fts_open(paths,FTS_LOGICAL|FTS_NOCHDIR,NULL);
    while((node=fts_read(tree))) {
        int fd;
        if (node->fts_info != FTS_F) continue;
        fd=open(node->fts_path,O_DIRECT);
        printf("Evicting from cache: %s\n", node->fts_path);
        posix_fadvise(fd,0,0,POSIX_FADV_DONTNEED);
        close(fd);
    }
    fts_close(tree);
    if (argc<2) free(paths[0]);
    free(paths);
    return(0);
}




