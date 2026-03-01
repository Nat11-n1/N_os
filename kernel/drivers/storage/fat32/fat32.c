#include "ata_pio\ata.h"
#include "lib\strcmp.h"

int bytes_per_sector=0;
int sectors_per_cluster=0;
int reserved_sectors=0;
int fat_count=0;
int sectors_per_fat=0;
int root_cluster=0;

int fat_start=0;
int data_start=0;

void fat32_init(){
    uint8_t buffer[512];
    ata_read_sector(0, buffer);
    bytes_per_sector = buffer[0x0B] | (buffer[0x0C] << 8);
    sectors_per_cluster = buffer[0x0D];
    reserved_sectors = buffer[0x0E] | (buffer[0x0F] << 8);
    fat_count = buffer[0x10];
    sectors_per_fat = buffer[0x24] | (buffer[0x25] << 8) | (buffer[0x26] << 16) | (buffer[0x27] << 24);
    root_cluster = buffer[0x2C] | (buffer[0x2D] << 8) | (buffer[0x2E] << 16) | (buffer[0x2F] << 24);

    fat_start  = reserved_sectors;
    data_start = reserved_sectors + (fat_count * sectors_per_fat);
}

void fat32_read_cluster(uint32_t cluster, uint8_t* buffer){
    int sector = data_start + ((cluster - 2) * sectors_per_cluster);
    for (int i = 0;i<sectors_per_cluster;i++){
        ata_read_sector(sector + i, buffer + (i * 512));
    }
}

uint32_t fat32_next_cluster(uint32_t cluster){
    uint8_t buffer[512];
    int fat_sector = fat_start + ((cluster * 4) / bytes_per_sector);
    int fat_offset = (cluster * 4) % bytes_per_sector;
    ata_read_sector(fat_sector,buffer);
    uint32_t next_cluster =buffer[fat_offset] | (buffer[fat_offset+1] << 8) | (buffer[fat_offset+2] << 16) | (buffer[fat_offset+3] << 24);
    return next_cluster;
}

void fat32_list_dir(uint32_t cluster){
    uint8_t buffer[512 * 8];//maximum size for a cluster in fat32
    fat32_read_cluster(cluster,buffer);
    for (int i = 0; i < sectors_per_cluster * 512;i+=32){
        if (buffer[i]==0x00){
            break;
        }
        else if (buffer[i]==0xE5){
            continue;
        }
        else{
            char name[12];
            for (int j = 0; j < 11; j++){
                name[j] = buffer[i + j];
            }
            name[11] = '\0';
            print_str((unsigned char*)name);
        }
    }
}

void fat32_read_file(uint32_t dir_cluster, char* filename, uint8_t* buffer){
    fat32_read_cluster(dir_cluster,buffer);
    for (int i = 0; i < sectors_per_cluster * 512;i+=32){
        if (buffer[i]==0x00){
            break;
        }
        else if (buffer[i]==0xE5){
            continue;
        }
        else{
            char name[12];
            for (int j = 0; j < 11; j++){
                name[j] = buffer[i + j];
            }
            name[11] = '\0';
            if (issamestr(name,filename)){
                uint32_t start_cluster = buffer[i+0x1A] | (buffer[i+0x1B] << 8) | (buffer[i+0x14] << 16) | (buffer[i+0x15] << 24);
                uint32_t cluster = start_cluster;
                while (cluster != 0x0FFFFFFF){
                    fat32_read_cluster(cluster, buffer);
                    cluster = fat32_next_cluster(cluster);
                }
            }
        }
    }
}