//Jacob Bennett & Dakota Smith
/*
	This program is a file system analysis tool that analyzes partitions formatted with the ext2 (revision 1) file system.
	It will read the given virtual disk and will either (1) Traverse entire directory structure starting with root, and 
	print all file/directory paths or (2) Print the content of a given file indicated with the provided <absolute-path>
*/

/*
  Notes:
  
	Two methods of execution:
	(1) Traverse entire directory structure starting with root, and print all file/directory paths:
			./fsa <diskname> <-traverse>
			Ex: ./fsa ext2_10mb.img -traverse
	(2) Print the content of a given file indicated with the provided <absolute-path>:
			./fsa <diskname> <-file> <absolute-path>
			Ex: ./fsa ext2_100mb.img -file /poems/alone/AlonePoemsAlonePoembyEdgarAllanPoe.txt
			
(1) Execute: ./fsa /home/jakenbake/Documents/CSE420/BlackBoxTesting4/ext2_10mb.img -traverse			
(2) Execute: ./fsa /home/jakenbake/Documents/CSE420/BlackBoxTesting4/ext2_10mb.img -file /media/jakenbake/ext2_10mb/searchdir/file1.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ext2fs/ext2fs.h>
#include <math.h>

unsigned int block_size = 0;
int fd, rv;
struct ext2_super_block *sb = NULL;
struct ext2_group_desc *gdt = NULL;
struct ext2_inode *inode = NULL;
struct ext2_dir_entry_2 *dir_entry = NULL;

void openDirectoryPrintEntities(int numBlocksForInodes, char *calling_dir) {
	//Seek to inode for directory that called function
	lseek(fd, block_size * (gdt->bg_inode_table) + sizeof(struct ext2_inode)*(dir_entry->inode - 1), SEEK_SET);
	read(fd, inode, sizeof(struct ext2_inode));

	struct ext2_dir_entry_2 *dir_entry2 = NULL;
	dir_entry2 = malloc(sizeof(struct ext2_dir_entry_2));
	memcpy(dir_entry2, dir_entry, sizeof(struct ext2_dir_entry_2));
	
	int positionFromStartOfBlock = 0;
	while(positionFromStartOfBlock <= inode->i_size) {
		//Seek to beginning of that inode's data blocks
		lseek(fd, (block_size * (inode->i_block[0])) + positionFromStartOfBlock, SEEK_SET);
		read(fd, dir_entry, sizeof(struct ext2_dir_entry));
		positionFromStartOfBlock += dir_entry->rec_len;
		if(dir_entry->name_len == 0) {break;}
		//f = first character of the current name	
		char f = dir_entry->name[0];
		if(strcmp(dir_entry->name,".") != 0 && strcmp(dir_entry->name,"..") && (f != '.') != 0) {
			char *new_dir = calloc(sizeof(char), (strlen(calling_dir) + strlen(dir_entry->name) + 2));
			strcat(new_dir, calling_dir);
			strcat(new_dir, "/");
			//strncat concatenate n bytes 
			strncat(new_dir,dir_entry->name, dir_entry->name_len);
			// strcat(new_dir, dir_entry->name);
			printf("%s\n", new_dir);

			// if directory then look at look at its iblock[] contents
			if(dir_entry->file_type == 2) {
			openDirectoryPrintEntities(numBlocksForInodes,new_dir);
			lseek(fd, block_size * (gdt->bg_inode_table) + sizeof(struct ext2_inode)*(dir_entry2->inode - 1), SEEK_SET);
			read(fd, inode, sizeof(struct ext2_inode));
			}
			free(new_dir);
		}
		
		//--Causes the weird symbols to print in the terminal
		// free(dir_entry2);
	}
}

void openDirectoryPrintFile(int numBlocksForInodes, char *calling_dir, char *searchFile)
{
	//Seek to inode for directory that called function
	lseek(fd, block_size * (gdt->bg_inode_table) + sizeof(struct ext2_inode)*(dir_entry->inode - 1), SEEK_SET);
	read(fd, inode, sizeof(struct ext2_inode));

	struct ext2_dir_entry_2 *dir_entry2 = NULL;
	dir_entry2 = malloc(sizeof(struct ext2_dir_entry_2));
	memcpy(dir_entry2, dir_entry, sizeof(struct ext2_dir_entry_2));
	
	int positionFromStartOfBlock = 0;
	while(positionFromStartOfBlock <= inode->i_size) {
		//Seek to beginning of that inode's data blocks
		lseek(fd, (block_size * (inode->i_block[0])) + positionFromStartOfBlock, SEEK_SET);
		read(fd, dir_entry, sizeof(struct ext2_dir_entry));
		positionFromStartOfBlock += dir_entry->rec_len;
		if(dir_entry->name_len == 0) {break;}
		//f = first character of the current name	
		char f = dir_entry->name[0];
		if(strcmp(dir_entry->name,".") != 0 && strcmp(dir_entry->name,"..") && (f != '.') != 0) {
			char *new_dir = calloc(sizeof(char),(strlen(calling_dir) + strlen(dir_entry->name) + 2));
			strcat(new_dir, calling_dir);
			strcat(new_dir, "/");
			strcat(new_dir, dir_entry->name);
			// printf("%s\n", new_dir);

			if(strcmp(new_dir,searchFile) == 0) // check if file inside at least 1 directory is searchFile
			{
				// print file contents. Get files inode and seek to beginning of data blocks for that inode. print file contents
				lseek(fd, block_size * (gdt->bg_inode_table) + sizeof(struct ext2_inode)*(dir_entry->inode - 1), SEEK_SET);
				read(fd, inode, sizeof(struct ext2_inode));
				lseek(fd, (block_size * (inode->i_block[0])), SEEK_SET);
				void *buf = malloc(sizeof(char) * inode->i_size);
				// no struct for file contents, just print file contents
				read(fd, buf, inode->i_size);
				printf("%s", (char*)buf);
				// read(fd, dir_entry, sizeof(struct ext2_dir_entry));
				// printf("%s\n", dir_entry->name);
				

				break;
			}


			// if directory then look at look at its iblock[] contents
			if(dir_entry->file_type == 2) {
				openDirectoryPrintFile(numBlocksForInodes,new_dir, searchFile);
				lseek(fd, block_size * (gdt->bg_inode_table) + sizeof(struct ext2_inode)*(dir_entry2->inode - 1), SEEK_SET);
				read(fd, inode, sizeof(struct ext2_inode));
			}
			free(new_dir);
		}
		
		//--Causes the weird symbols to print in the terminal
		//free(dir_entry2);
	}







}

int main(int argc, char *argv[]) {
 
	fd = open(argv[1], O_RDONLY);
	if(fd == -1) {
		perror("disk_image_file open failed");
		exit(1);
	}

	//Skip boot info (first 1024 bytes)
	if(lseek(fd, 1024, SEEK_CUR) != 1024) {
		perror("File seek failed to skip the boot info");
		exit(1);
	}

	//Allocate memory for sb, gdt, inode, dir_entry
	sb = malloc(sizeof(struct ext2_super_block));
	if(sb == NULL) {
		fprintf(stderr, "%s: Error in malloc for superblock struct\n", "sb");
		exit(1);
	}
	gdt = malloc(sizeof(struct ext2_group_desc));
	if(gdt == NULL) {
		fprintf(stderr, "%s: Error in malloc for group descriptor struct\n", "sb");
		exit(1);
	}
	inode = malloc(sizeof(struct ext2_inode));
	if(inode == NULL) {
		fprintf(stderr, "%s: Error in malloc for inode struct\n", "sb");
		exit(1);
	}
	dir_entry = malloc(sizeof(struct ext2_dir_entry));
	if(dir_entry == NULL) {
		fprintf(stderr, "%s: Error in malloc for dir_entry struct\n", "sb");
		exit(1);
	}

	//Read whole superblock and load into the ext2_super_block struct
	rv = read(fd, sb, sizeof(struct ext2_super_block));
  	if(rv == -1) {
   		perror("File read failed");
   		exit(1);
  	}
  	if(rv == 1024) {
  
    //--Test--------------------------------------------------------------------------------------------------
  		// printf("\nTest:\n");
		// print the s_inodes_count
		// printf("Total Number of Inodes: %u\n", sb->s_inodes_count);
		// print the s_first_data_block
		// printf("The first data block is at block %u\n", sb->s_first_data_block);
		//Print total number of blocks per group
		// printf("Total number of blocks per group is %u\n", sb->s_blocks_per_group);
		// print block size
		block_size = 1024 << sb->s_log_block_size;
		// printf("Block size: %d\n", block_size);
		
		// skip to the first block group descriptor
		lseek(fd,block_size, SEEK_SET);
		read(fd, gdt, sizeof(struct ext2_group_desc));

		// seek to the root inode
		// print gdt->bg_inode_table
		// printf("block id for inode table: %u\n", gdt->bg_inode_table);
		lseek(fd, ((block_size * gdt->bg_inode_table) + sizeof(struct ext2_inode)), SEEK_SET);
		read(fd, inode, sizeof(struct ext2_inode));
		



		// print the root inode's i_mode
		// printf("Root inode's i_mode: %u\n", inode->i_mode);
		// print the root inode's i_size
		// printf("Root inode's i_size: %u\n", inode->i_size);
		// print the root inode's i_blocks
		// printf("Root inode's i_blocks: %u\n", inode->i_blocks);	
		// printf("\n\n");
	//--End Test-----------------------------------------------------------------------------------------------	
		
		
		
		//Find how many inodes can fit in a block
		int numInodesPerBlock = block_size / sizeof(struct ext2_inode);
		//Find how many blocks are needed to store the inodes
		int numBlocksForInodes = ceil((sb->s_inodes_per_group / numInodesPerBlock));
	
		//Traverse entire directory structure and print all file/directory paths
		if(strcmp(argv[2], "-traverse") == 0) {
			int positionFromStartOfBlock = 0;
			printf("/\n");
			while(positionFromStartOfBlock <= inode->i_size) 
			{
				lseek(fd, block_size * (gdt->bg_inode_table + numBlocksForInodes) + positionFromStartOfBlock, SEEK_SET);
				read(fd, dir_entry, sizeof(struct ext2_dir_entry));
				positionFromStartOfBlock += dir_entry->rec_len;
				if(dir_entry->rec_len == 0){break;}
				//f = first character of the current name
				char f = dir_entry->name[0];
				if(strcmp(dir_entry->name,".") != 0 && strcmp(dir_entry->name,"..") && (f != '.') != 0) {
					printf("/%s\n", dir_entry->name);
					char *calling_dir = malloc(sizeof(char) * (strlen(dir_entry->name) + 1));
					strcat(calling_dir,"/");
					strcat(calling_dir, dir_entry->name);
					// printf("calling dir is %s\n", calling_dir);
					// printf("\tinode: %d\n", dir_entry->inode);
					// if directory then look at look at its iblock[] contents
					if(dir_entry->file_type == 2) {
						openDirectoryPrintEntities(numBlocksForInodes, calling_dir);
						lseek(fd, ((block_size * gdt->bg_inode_table) + sizeof(struct ext2_inode)), SEEK_SET);
						read(fd, inode, sizeof(struct ext2_inode));
					}
					free(calling_dir);
				}
			}
  		}
	
		//Print the contents of the given file
		if(strcmp(argv[2], "-file") == 0) {
			// seek to the root inode
			lseek(fd, ((block_size * gdt->bg_inode_table) + sizeof(struct ext2_inode)), SEEK_SET);
			read(fd, inode, sizeof(struct ext2_inode));
			
			int positionFromStartOfBlock = 0;
			// printf("/\n");
			while(positionFromStartOfBlock <= inode->i_size) 
			{
				lseek(fd, block_size * (gdt->bg_inode_table + numBlocksForInodes) + positionFromStartOfBlock, SEEK_SET);
				read(fd, dir_entry, sizeof(struct ext2_dir_entry));
				positionFromStartOfBlock += dir_entry->rec_len;
				if(dir_entry->rec_len == 0){break;}
				//f = first character of the current name
				char f = dir_entry->name[0];
				if(strcmp(dir_entry->name,".") != 0 && strcmp(dir_entry->name,"..") && (f != '.') != 0) {
					// printf("/%s\n", dir_entry->name);
					char *calling_dir = malloc(sizeof(char) * (strlen(dir_entry->name) + 1));
					strcat(calling_dir,"/");
					strcat(calling_dir, dir_entry->name);
					char *searchFile = argv[3];
					if(strcmp(calling_dir,searchFile) == 0) // check for files in root directory that match argv[3]
					{
						// print file contents
						// break
					}

					// if directory then look at look at its iblock[] contents
					if(dir_entry->file_type == 2) {
						openDirectoryPrintFile(numBlocksForInodes, calling_dir, searchFile);
						lseek(fd, ((block_size * gdt->bg_inode_table) + sizeof(struct ext2_inode)), SEEK_SET);
						read(fd, inode, sizeof(struct ext2_inode));
					}
					free(calling_dir);
				}
			}
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			//--Print contents code here
			/* Test print file contents:	
			printf("\n");	
			int c;	
 			FILE *fileptr = fopen(argv[3], "r");		
 			//While not at end of file
 			while(!feof(fileptr)) {
 				//Print every character of the file
 				c = fgetc(fileptr);
 				if(feof(fileptr))		break;		
 				printf("%c", c);
 			}	
 			fclose(fileptr);
			printf("\n");
		 */ 	   
		}
	}
 	free(sb);
	free(gdt);
	free(inode);
	free(dir_entry);
	
	close(fd);
  
	return 0;
}
