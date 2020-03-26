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
void print_mbr_partition_info(struct mbr mbr_entity);
int main(int argc, char* argv[]){
    FILE * fd;
    fd = fopen(argv[1], "r+b");

    int partitions = atoi(argv[2]);
    int file_size = getFileSize(argv[1]);
    int sectors_len = (file_size / 512) / partitions;
    std::cout << "[!]Block len at sectors: " << sectors_len << std::endl;

    if (partitions <= 4){
        std::cout << "[!]Creating MBR entity..." << std::endl;
        struct mbr mbrr = create_mbr(partitions, true, sectors_len);
        std::cout <<"Fwrite = "<< fwrite(&mbrr, sizeof(mbrr), 1, fd) << std::endl;
    } else {
        std::cout << "[!]Creating MBR-EBR entity..." << std::endl;
        struct mbr ebr_entity = create_mbr(3, true, sectors_len);
        ebr_entity.partition[3].partition_type = 0x0f;
        ebr_entity.partition[3].first_sector_lba = 1+sectors_len*3;
        ebr_entity.partition[3].sectors = (file_size / 512) - 3;
        fwrite(&ebr_entity, sizeof(ebr_entity), 1, fd);
        fseek(fd, SECTOR_SIZE*(1+sectors_len*3), SEEK_SET);
        for (int i = 4 ; i <= partitions; i++){
            struct mbr ebr = create_mbr(2, false, sectors_len);
            ebr.signature = 0x00;
            //первая часть - описатель себя
            ebr.partition[0].partition_type = 0x01;
            ebr.partition[0].first_sector_lba = 1;
            ebr.partition[0].sectors = sectors_len;
            //вторая часть - указатель на след цель
            if (i != partitions){
                ebr.partition[1].partition_type = 0x0f;
                ebr.partition[1].first_sector_lba = (i)*sectors_len - (1+sectors_len*3);
                ebr.partition[1].sectors = sectors_len;
            } else {
                ebr.partition[1].partition_type = 0;
                ebr.partition[1].first_sector_lba = 0;
                ebr.partition[1].sectors = 0;
            }
            fwrite(&ebr, sizeof(ebr), 1, fd);
            fseek(fd, SECTOR_SIZE*(i)*sectors_len ,SEEK_SET);
        }

    }
    fclose(fd);
    std::cout<<"[+]Done"<<std::endl;
    return 0;
}

struct mbr create_mbr(int partitions, int is_bootable, int sectors_len){
    struct mbr *new_mbr;

    new_mbr = (struct mbr *)malloc(sizeof(struct mbr));
    memset(new_mbr->code, 0, sizeof(new_mbr->code));
    new_mbr->signature = 0xDEADCAFE;
    new_mbr->vvoid     = 0;
    if (partitions <= 4 ){
        for (int i = 0; i<partitions; i++){
            //std::cout<<"Creating partition " << i << std::endl;
            new_mbr->partition[i].status         = (is_bootable != 0 && i == 0) ? BOOTABLE_MBR : 0;
            new_mbr->partition[i].first_sector   = (struct chs){0, 0, 0};
            new_mbr->partition[i].partition_type = 0x01;
            new_mbr->partition[i].end_sector     = (struct chs){0, 0, 0};
            new_mbr->partition[i].first_sector_lba = 1+sectors_len*i;
            new_mbr->partition[i].sectors = sectors_len;
        }
    }
    new_mbr->mbr_signature = MBR_SIGNATURE;

    return *new_mbr;
}

int getFileSize(const char * fd){
    struct stat stat_buf;
    int res = stat(fd, &stat_buf);
    return stat_buf.st_size;
}

