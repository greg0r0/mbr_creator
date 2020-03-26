#ifndef __MBR_H__
#define __MBR_H__

#define MBR_SIGNATURE 0xaa55
#define BOOTABLE_MBR 0x80

struct chs {
    int cylinder;
    int head;
    int sector;
};

struct mbr{
    char code[440];
    int signature;
    int vvoid;
    struct mbr_patrition {
        int status;
        struct chs first_sector;
        int partition_type;
        struct chs end_sector;

        unsigned int first_sector_lba;
        unsigned int sectors;
    } partition[4];
    int mbr_signature;
}

#endif 