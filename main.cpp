#include <iostream>
#include <cstring>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include "mbr.h"


struct lol
{
    int a;
    char b;
};

struct mbr create_mbr(int partitions, int is_bootable, int block_size);
int getFileSize(const char *fd);

int main(int argc, char* argv[]){
    FILE * fd;
    fd = fopen(argv[1], "r+b");

    int file_size = getFileSize(argv[1]);


    struct mbr mbrr = create_mbr(atoi(argv[2]), true, file_size);
    fwrite(&mbrr, sizeof(mbrr), 1, fd);

    fclose(fd);
    return 0;
}

struct mbr create_mbr(int partitions, int is_bootable, int file_size){
    int sectors_len = ((file_size / 512)) / partitions;

    std::cout << "Sectors per partition: "<<sectors_len << std::endl;
    struct mbr *new_mbr;

    new_mbr = (struct mbr *)malloc(sizeof(struct mbr));
    memset(new_mbr->code, 0, sizeof(new_mbr->code));
    new_mbr->signature = 0x0;
    new_mbr->vvoid     = 0;
    if (partitions <= 4 ){
        for (int i = 0; i<partitions; i++){
            std::cout<<"Creating partition " << i << std::endl;
            new_mbr->partition[i].status         = (is_bootable != 0 && i == 0) ? BOOTABLE_MBR : 0;
            new_mbr->partition[i].first_sector   = (struct chs){0, 0, 0};
            new_mbr->partition[i].partition_type = 0x01;
            new_mbr->partition[i].end_sector     = (struct chs){0, 0, 0};
            new_mbr->partition[i].first_sector_lba = 1+sectors_len*i;
            new_mbr->partition[i].sectors = sectors_len;
        }
    } else {
        *new_mbr = create_mbr(4, is_bootable, file_size);
    }
    new_mbr->mbr_signature = MBR_SIGNATURE;

    return *new_mbr;
}

int getFileSize(const char * fd){
    struct stat stat_buf;
    int res = stat(fd, &stat_buf);
    return stat_buf.st_size;
}