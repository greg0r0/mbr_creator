#ifndef __MBR_H__
#define __MBR_H__

#define MBR_SIGNATURE 0xaa55
#define BOOTABLE_MBR 0x80
#define SECTOR_SIZE 512



struct chs {
    unsigned char cylinder;
    unsigned char head;
    unsigned char sector;
} __attribute__((packed));

struct mbr{
    char code[440];
    unsigned int signature;
    short int vvoid;
    struct mbr_patrition {
        unsigned char status;
        struct chs first_sector;
        unsigned char partition_type;
        struct chs end_sector;

        unsigned int first_sector_lba;
        unsigned int sectors;
    } partition[4];
    unsigned short int mbr_signature;
} __attribute__((packed));

#endif 