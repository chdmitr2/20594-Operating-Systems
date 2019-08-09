/*****************************************************************************

   Written by:  Chudnovsky Dmitriy, ID: 324793900

   Description: This programs formats a file system to FAT12.

 ****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <linux/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "fat12.h"

int fid; /* global variable set by the open() function */

/*Mapping disk data directly to structures like in fat-12 file system documents*/
/*Open the file of diskimage and then  write its contents as full (512-byte) disk sectors*/
int fd_write(int sector_number, char *buffer)
            {
             int dest, len;
             int bps=DEFAULT_SECTOR_SIZE;
             dest = lseek(fid, sector_number * DEFAULT_SECTOR_SIZE, SEEK_SET);
             if(dest != sector_number * bps)
               { 
                printf("WRITING ERROR");/* Error handling */
                exit(one);
               }
             len = write(fid, buffer, bps);
             if(len != bps)
               {  
                printf("WRITING ERROR");/* Error handling */
                exit(one);
               }
             return len;
            }

int main(int argc, char *argv[])      
        {
         if(argc != two)
           {
            printf("Usage: %s <floppy_image>\n", argv[zero]);
	    exit(one);
           }
         if((fid = open (argv[one], O_RDWR|O_CREAT, 0644))  < zero )
	   {
	    perror("Error: ");
            return minus;
           }

	// Step 1. Create floppy.img with the school solution. Read the values 
	// from the boot sector of the floppy.img and initialize boot sector
	// with those values
        // Set boot sector
        int i=zero;
        boot_record_t boot;
        boot.sector_size=DEFAULT_SECTOR_SIZE;
        boot.sectors_per_cluster=one;
        boot.reserved_sector_count=one;
        boot.number_of_fats=two;
        boot.number_of_dirents=four*eight*seven;
        boot.sector_count=SQR(eight)*nine*five;
        boot.media_type=hex_number1;
        boot.fat_size_sectors=nine;
        boot.sectors_per_track=two*nine;
        boot.nheads=two;
        boot.sectors_hidden=zero;
        boot.sector_count_large=zero;
        FOR(i,zero,seven){boot.oem_id[i] = zero;}
	
        if(read(fid, &boot, sizeof (boot)) > zero)
          {
	   printf("sector_size: %d\n", boot.sector_size);
	   printf("sectors_per_cluster: %d\n", boot.sectors_per_cluster);
	   printf("reserved_sector_count: %d\n", boot.reserved_sector_count);
	   printf("number_of_fats: %d\n", boot.number_of_fats);
	   printf("number_of_dirents: %d\n", boot.number_of_dirents);
           printf("sector_count: %d\n", boot.sector_count);
	  }
             
        int index_sector=zero;
        int sector1=one;
        int sector2=two*nine; 
        char buf1[DEFAULT_SECTOR_SIZE]={zero};
        char zero_buf[DEFAULT_SECTOR_SIZE]={zero};
        memcpy(buf1,&boot,sizeof(boot));// Copy boot sector 
        fd_write(zero,buf1);// Writes the boot sector to the image

        // Step 2. Set FAT1/FAT2 table entires to 0x0000 (unused cluster)
        char buf2[DEFAULT_SECTOR_SIZE] = { hex_number1, hex_number2, hex_number2 }; // EOC (0xFFF)  
        FOR(index_sector,sector1,sector2)//Going from first to last sector entries
           {((index_sector % nine) == one) ? fd_write(index_sector,buf2) : fd_write(index_sector,zero_buf);} 
             
        // Step 3. Set direntries as free (0xe5) according to the fat12.pdf.
        // While zeroing dentries will also work, we prefer a solution
        // that mark them free. In that case it will be possible to perform
        // unformat operation. 
        // Create the root directory.

        FOR(index_sector,sector2+one,sector2+six+seven) {fd_write(index_sector, zero_buf);}

        // Step 4. Handle data block (e.g you can zero them or just leave
        // untouched. What are the pros/cons?)
        // Create rest directory entries.

        FOR(index_sector,sector2+one,SQR(eight)*nine*five-one) {fd_write(index_sector, zero_buf);}
        close(fid);
        return zero;
       }



