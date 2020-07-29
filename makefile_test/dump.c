#include <stdio.h>
#include <string.h>
#include <sys/vfs.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#define FILEPATHMAX 80
#define SAVE_DATA_ITER 20   //保存文件次数

//dump ddr_base_addr length -s [split_size] -n [base_name] -l d[location]

typedef enum EMDiskSizeType_{
	TOTAL_SIZE,
	FREE_SIZE
} EMDiskSizeType;

unsigned long long get_diskSize(char *strDir, EMDiskSizeType  disk_type){

	struct statfs diskinfo;
	unsigned long long llcount = 0;

	if(statfs(strDir,&diskinfo) == -1){
		printf(" errrrrr \n");
		return 0;

	}else{
		switch (disk_type)
		{
		case TOTAL_SIZE:
			llcount = diskinfo.f_blocks * diskinfo.f_bsize;
			llcount =llcount >> 20;
			break;
		case FREE_SIZE:
			llcount = diskinfo.f_bfree * diskinfo.f_bsize;
			llcount = llcount >> 20;
			break;		
		default:
			break;
		}		
		return llcount;
	}

}

//save data 
int dump_data_save(unsigned int start, unsigned char *data, unsigned int len, int file_num){
	
	char * save_dir = "/home/chao-zhang/linux-arm-test/makefile_test";
	unsigned char *pu8 = NULL;
	char *full_file_name = NULL;
	char *cmd_cmmand = NULL;
	
	FILE *fp = NULL;
		
	full_file_name = (char*)malloc(FILEPATHMAX);
	cmd_cmmand = (char*)malloc(FILEPATHMAX);
	
	unsigned long long disk_space_sount =  get_diskSize(save_dir,FREE_SIZE);
	
	if(disk_space_sount<=10){
		printf(" %s: Lack of space, free: %llu\n",save_dir,disk_space_sount);
		return -1;
	}

	if(start == 0){

		pu8 = (unsigned char *)data;		
		sprintf(full_file_name,"%s/%d.txt",save_dir,file_num);   //
		fp = fopen(full_file_name,"wb");  //create the numbered file

		if(fp == NULL){
			printf("file:%s opened err\n",full_file_name);
		}
		//printf("file:%s created OK\n",full_file_name);
		fwrite(pu8,sizeof(char),len,fp);
		sprintf(cmd_cmmand,"%s %d.tar %d.txt","tar -czvf",file_num,file_num);  
		system(cmd_cmmand);													   //tar -czvf ***.tar ***.bin
		sprintf(cmd_cmmand,"%s %s","rm ",full_file_name);
		system(cmd_cmmand);			                                           //rm ***.bin
		fclose(fp);
		fp = NULL;
	}
		
		free(full_file_name);
		free(cmd_cmmand);
		return 0;
}

//分成10M进行存储
void data_separation(unsigned char *data, unsigned int total_size){

	unsigned char *data_separated = NULL;
	unsigned data_offset = 0;
	unsigned int file_num;
	for(file_num = 1; file_num <= SAVE_DATA_ITER; file_num++){

		data_separated = (unsigned char*)malloc(total_size/SAVE_DATA_ITER);
		memcpy(data_separated,data+data_offset,total_size/SAVE_DATA_ITER);
		
		if(dump_data_save(0,data_separated,total_size/SAVE_DATA_ITER,file_num) != 0){
			printf("save data err\n");
			break;
		};
	
		printf("==========> data_offset: 0x%x file_num:%d data addr: 0x%x\n",data_offset,file_num,data+data_offset);
		free(data_separated);
		data_offset = file_num * total_size/SAVE_DATA_ITER;
	}
}


int parse_addr_option();
int parse_save_option();
int pares_name_option();

int main(int argc,char *argv[]){


	int cmd = 0;

	if(argc<2){
		printf("err:too few parameters! -a -l are required!\n");
		printf("Usage:\n");
		printf("-a:base addrress -l: data lenth\n");
		printf("[-lo:location] [-s:split size] [-n: name] \n");
		return 1;
	}

	while((cmd = getopt(argc, argv,"a:l:lo::s::n::"))!=-1){
		printf("argc: %d\n",argc);

		switch (cmd)
		{
		case 'a':
			printf("addr: %s\n",optarg);
			break;
		case 'l':
			printf("len: %s\n",optarg);
			break;
		
		default:
			break;
		}
	}

	/*
	const unsigned int total_size = 1024*1024*200;    //100MB
	char data[total_size];
	//char *data=malloc(1024*1024*100);
	for(int i = 0; i<total_size; i++){
		data[i] = i%255;
	}
	data_separation(data,total_size);
	*/

}
