#include <stdio.h>
#include <string.h>
#include <sys/vfs.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <dirent.h>

#define FILEPATHMAX 80
#define NAMEMAX 20
#define SAVE_DATA_ITER 20   //保存文件次数
#define SPLITE_SIZE 20
#define MAX_DATA_SIZE 1024*1024*200




extern char *optarg;
extern int optind,opterr,optopt;
struct file_property{
	unsigned long long base_addr;    			//base address of data
	unsigned int data_length;   			//lenth of data to be saved
	int split_size;			   			//sector capacity
	char file_save_path[FILEPATHMAX];  	//file save path
	char name[NAMEMAX];                 //name
};
struct file_property fileproperty = {
	.base_addr = 0,
	.data_length = 0,
	.split_size = SPLITE_SIZE,
	.name = "data",
	.file_save_path = "cl", //current location
};
static int isdigitstr(char *str)
{
	return (strspn(str, "0123456789")==strlen(str)); 
}
//dump ddr_base_addr length -s [split_size] -n [base_name] -l d[location]
typedef enum EMDiskSizeType_{
	TOTAL_SIZE,
	FREE_SIZE
} EMDiskSizeType;
typedef enum ERRCODE{	
	NOT_NUMBER = -2,
	DATA_TOOBIG,
	FEW_PARAMETERS,
	FEW_MEMORY,
	PATH_NOTFOUND,
	OK,
} err_code;
static unsigned long long get_diskSize(char *strDir, EMDiskSizeType  disk_type){

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
static int dump_data_save(unsigned int start, unsigned char *data, unsigned int len, int file_num){
	
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
static void data_separation(unsigned char *data, unsigned int total_size){

	unsigned char *data_separated = NULL;
	unsigned int data_offset = 0;
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

static int  data_dumped(unsigned char *data_started, unsigned int data_len, char* name){
		


	return 0;
}

static int data_separated_dump(struct file_property fp){
	unsigned char* data_started = NULL;
	unsigned int data_offset = 0;
	unsigned int file_num;

	//for()
	



}

int ap_query_cp_memory(unsigned int offset,unsigned int length){
	int fd = -1;
	void *map_addr = NULL;
	if((offset + length) >= MAX_DATA_SIZE){
		return OK;
	}




}



//path and disksize check
static int parameter_cheak(struct file_property *fp){
	char cuurent_path[FILEPATHMAX];
	DIR *d = NULL;
	unsigned long long per_data_size;   //per data size
	
	getcwd(cuurent_path,FILEPATHMAX);
	
	if(strcmp(fp->file_save_path,"cl")){
		d = opendir(fp->file_save_path);
		if(d == NULL){
			printf("path not right, default path (current) is used!\n");
			strcpy(fp->file_save_path,cuurent_path);
			closedir(d);
			//return PATH_NOTFOUND;
		}
	}else{
		strcpy(fp->file_save_path,cuurent_path);
	}
	
	unsigned long long disk_space_sount =  get_diskSize(fp->file_save_path,FREE_SIZE);
	per_data_size = fp->data_length/fp->split_size; 
	if((disk_space_sount<=10)||(disk_space_sount < per_data_size)){
		printf(" %s: Lack of space, free: %llu\n",fp->file_save_path,disk_space_sount);
		return FEW_MEMORY;
	}
	printf(" %s: Lack of space, free: %lluM\n",fp->file_save_path,disk_space_sount);
	return OK;
}

static int parse_path_option(char* file_path,struct file_property *fp){
	
	strcpy(fp->file_save_path,file_path);
	return OK;
}
//hex only default
static int parse_data_addr_option(char *data_addr, struct file_property *fp){
	unsigned long long data_base_addr = 0;
	sscanf(data_addr,"%llx",&data_base_addr); 	
	fp->base_addr = data_base_addr;
	return OK;
}
static int parse_data_length(char *data_len,struct file_property *fp){
	unsigned int data_lenth = 0;
	if(!isdigitstr(data_len)){
		printf("ERR: -l : NUMBERS ONLY\n");
		return NOT_NUMBER;
	}
	data_lenth = atoi(data_len);
	if(data_lenth>1024*1024*400){
		printf("ERR: length should smaller than 400MB\n");
		return DATA_TOOBIG;
	}
	fp->data_length = data_lenth;
	return OK;
}
static int parse_slpit_option(char *split_size, struct file_property *fp){
	int int_split_size = -1;
	if(!isdigitstr(split_size)){
		printf("ERR: -s : NUMBERS ONLY\n");
		return NOT_NUMBER;
	}
	int_split_size = atoi(split_size);
	fp->split_size = int_split_size;
	return OK;
}
static int pares_name_option(char *name_option,struct file_property *fp){
	strcpy(fp->name,name_option);
	return OK;
}

int main(int argc,char *argv[]){
	/*
	unsigned long data_addr = 0;
	unsigned int data_lenth = 0;
	//char *addr = NULL;
	int data_test = 9898;
	int *p=NULL;
	//unsigned long a = 0xffffffffeeeffffffff;
	char *addr = "eeeeefffffffff";
	//addr = (char *)malloc(sizeof(&data_test));
	//printf("addr data_test 0x%x\n", &data_test); 
	//sprintf(addr,"%x",&data_test);
	//printf("addr : 0x%x context is :%d\n",&data_test,*(&data_test));
	printf("addr test %s\n", addr);  //将addr转换成字符串地址
	sscanf(addr,"%llx",&data_addr);    //将字符串地址解析回去	
	printf("data_addr int 0x%llx\n",data_addr);	

	//printf("-----------------------%llx\n",a);
	//free(addr);
*/
	struct file_property fp_set;
	fp_set = fileproperty;

	int ret = 0;
	int cmd = 0;
	if(argc<2){	
		printf("Usage:\n");
		printf("-a:base addrress (HEX) -l: data lenth\n");
		printf("[-d:location] [-s:split number] [-n: name] \n");
		return FEW_PARAMETERS;
	}	
	//printf("pid : %d\n",getpid());
	while((cmd = getopt(argc, argv,"a:l:d:s:n:"))!=-1){  //getopt will retrn -1 when all the parameters are listed
		switch (cmd)
		{
		case 'a':
			if(optarg){
				fileproperty.base_addr = parse_data_addr_option(optarg,&fp_set);
			}else{
				printf("empty option -%c...\n",cmd);
				exit(0);
			}
			break;
		case 'l':
			if(optarg){
				ret = parse_data_length(optarg,&fp_set);
				if(ret != OK){
					exit(6);
				}	
				fileproperty.data_length = ret;
			}else{
				printf("empty option -%c...\n",cmd);
				exit(7);
			}
			break;

			//optional command
		case 'd': //save path set
			if(optarg){
				ret = parse_path_option(optarg,&fp_set);
				if(ret != OK){
					exit(6);
				}
			} 
			else{
				printf("empty option -%c...\n",cmd);
				exit(7);
			}
			break;
		case 's':  //split size
			if(optarg){
				ret = parse_slpit_option(optarg,&fp_set);
				if(ret!=OK){
					exit(6);
				}	
				fileproperty.split_size = ret;
			}else{
				printf("empty option -%c...\n",cmd);
				exit(7);
			}
			break;
		case 'n':  //name set
			if(optarg){
				ret = pares_name_option(optarg,&fp_set);
				if (ret!= OK)
				{
					exit(6);
				}
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
	if((fp_set.base_addr < 0 )||(fp_set.data_length<=0)){
		printf("ERR: option data_addr(-a) and data_lenth(-l) are required!\n");
		return -1;
	}
	
	
	//option parameter check
	if(parameter_cheak(&fp_set)!=OK) return -1;
	
	long page_size = sysconf(_SC_PAGESIZE);
	printf("page size of this pc : %lu\n",page_size);
	fileproperty = fp_set;
	
	printf("==============>file save   path: %s\n",fileproperty.file_save_path);
	printf("==============>file save length: %d\n",fileproperty.data_length);
	printf("==============>file split  size: %d\n",fileproperty.split_size);
	printf("==============>file split  name: %s\n",fileproperty.name);
	printf("==============>file base   addr: 0x%llx\n",fileproperty.base_addr);

	printf(":)\n");
	
	
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
