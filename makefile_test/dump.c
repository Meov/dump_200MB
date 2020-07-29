#include <stdio.h>
#include <string.h>
#include <sys/vfs.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#define FILEPATHMAX 80
#define NAMEMAX 20
#define SAVE_DATA_ITER 20   //保存文件次数
#define SPLITE_SIZE 20
extern char *optarg;
extern int optind,opterr,optopt;
struct file_property
{
	unsigned int base_addr;    			//base address of data
	unsigned int data_lenth;   			//lenth of data to be saved
	int split_size;			   			//sector capacity
	char file_save_path[FILEPATHMAX];  	//file save path
	char name[NAMEMAX];                 //name
};
struct file_property fileproperty = {
	.base_addr = 0,
	.data_lenth = 0,
	.split_size = SPLITE_SIZE,
	.name = "data",
	.file_save_path = "cl", //current location
};
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
	
		//printf("==========> data_offset: 0x%x file_num:%d data addr: 0x%x\n",data_offset,file_num,data+data_offset);
		free(data_separated);
		data_offset = file_num * total_size/SAVE_DATA_ITER;
	}
}
char* parse_path_option(char* file_path){
	return file_path;
}
//hex only default
unsigned int parse_data_addr_option(char *data_addr){	
	unsigned int data_base_addr = 0;
	sscanf(data_addr,"%x",&data_base_addr); 
	return data_base_addr;
}
int parse_data_lenth(char *data_len){
	int data_lenth = 0;
	data_lenth = atoi(data_len);
	return data_lenth;
}
int parse_slpit_option(char *split_size){
	int int_split_size = 0;
	int_split_size = atoi(split_size);
	return int_split_size;
}
char* pares_name_option(char *name_option){
	return name_option;
}

int main(int argc,char *argv[]){
	char *file_save_path = NULL;
	unsigned int total_size = 0;
	
	
	/*
	
	int data_addr = 0;
	unsigned int data_lenth = 0;
	char *addr = NULL;
	int data_test = 9898;
	int *p=NULL;

	//p = (int *)malloc(int);


	addr = (char *)malloc(sizeof(&data_test));
	
	printf("addr data_test 0x%x\n", &data_test); 
	sprintf(addr,"%x",&data_test);

	printf("addr : 0x%x context is :%d\n",&data_test,*(&data_test));
	printf("addr test %s\n", addr);  //将addr转换成字符串地址
	
	
	sscanf(addr,"%x",&data_addr);    //将字符串地址解析回去	
	
	p = (unsigned int *)data_addr;
	
	printf("data_addr int 0x%8x\n", *(int *)data_addr);	
	printf("the context of 0x%x is :%x\n",data_addr,*p);
	
	
	free(addr);
	

*/
	unsigned int ret = 0;
	int cmd = 0;
	if(argc<2){	
		printf("Usage:\n");
		printf("-a:base addrress -l: data lenth\n");
		printf("[-lo:location] [-s:split number] [-n: name] \n");
		return 1;
	}	
	//printf("pid : %d\n",getpid());
	while((cmd = getopt(argc, argv,"a:l:d:s:n:"))!=-1){  //getopt will retrn -1 when all the parameters are listed
		switch (cmd)
		{
		case 'a':
			if(optarg){
				ret = parse_data_addr_option(optarg);
				if(ret<0)
					exit(6);
				fileproperty.base_addr = ret;
				printf("base       addr: %x\n",fileproperty.base_addr);
			}else{
				printf("empty option -%c...\n",cmd);
				exit(7);
			}
			break;
		case 'l':
			if(optarg){
				printf("data      lenth: %s\n",optarg);
				ret = parse_data_lenth(optarg);
				if(ret<0)
					exit(6);
				fileproperty.data_lenth = ret;
			}else{
				printf("empty option -%c...\n",cmd);
				exit(7);
			}
			break;
			//optional command
		case 'd': //save path set
			
			if(optarg){
				strcpy(fileproperty.file_save_path,parse_path_option(optarg));
				printf("target location: %s\n",fileproperty.file_save_path);
			} 
			else{
				printf("empty option -%c...\n",cmd);
				exit(7);
			}
			break;
		case 's':  //split size
			if(optarg){
				ret = parse_slpit_option(optarg);
				if(ret<0)
					exit(6);
				fileproperty.split_size = ret;
				printf("split    number: %d\n",fileproperty.split_size);
			}else{
				printf("empty option -%c...\n",cmd);
				exit(7);
			}
			break;
		case 'n':  //name set
			if(optarg){
				strcpy(fileproperty.name,pares_name_option(optarg));
				printf("name ===> prefix: %s\n",fileproperty.name);
			}else{
				printf("empty option -%c...\n",cmd);
				exit(7);
			}
			break;
		default:
			break;
		}
	}

	//check the addr and lent infomation
	if((fileproperty.base_addr < 0 )||(fileproperty.data_lenth<=0)){
		if(file_save_path != NULL) free(file_save_path);
		printf("ERR! data_addr and data_lenth are required!\n");
		printf("-a: data base addr, -l: data_lenth\n");
		return -1;
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
