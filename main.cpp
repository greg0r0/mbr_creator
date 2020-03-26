#include <iostream>
#include <cstring>
#include <stdio.h>
#include "mbr.h"


struct lol
{
    int a;
    char b;
};

struct mbr create_mbr(int partitions, int is_bootable);

int main(int argc, char* argv[]){

    struct mbr mbrr = create_mbr(1, true);

    std::cout<<sizeof(struct mbr)<<std::endl;
    FILE * fd;

    fd = fopen(argv[1], "r+b");

    fwrite(&mbrr, sizeof(mbrr), 1, fd);

    fclose(fd);
    return 0;
}

struct mbr create_mbr(int partitions, int is_bootable){
    struct mbr *new_mbr;

    new_mbr = (struct mbr *)malloc(sizeof(struct mbr));
    memset(new_mbr->code, 0, sizeof(new_mbr->code));
    new_mbr->signature = 0x0;
    new_mbr->vvoid     = 0;
    if (partitions <= 4 ){
        for (int i =0; i<partitions; i++){
            new_mbr->partition[i].status         = (is_bootable != 0) ? BOOTABLE_MBR : 0;
            new_mbr->partition[i].first_sector   = (struct chs){0, 0, 0};
            new_mbr->partition[i].partition_type = 0x01;
            new_mbr->partition[i].end_sector     = (struct chs){0, 0, 0};
            new_mbr->partition[i].first_sector_lba = 0;
            new_mbr->partition[i].sectors = 0;
        }
    } else {
        *new_mbr = create_mbr(4, is_bootable);
    }
    new_mbr->mbr_signature = MBR_SIGNATURE;

    return *new_mbr;
}