#include <stdio.h>
#include <string.h>
#include <sys/vfs.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#define FILEPATHMAX 80
#define MAX_NAME 80
#define SPLITE_SIZE 20  //defualt
#define MAX_DATA_SIZE 1024*1024*200
extern char *optarg;
extern int optind,opterr,optopt;
struct file_property{
	unsigned long long base_addr;    		//base address of data
	unsigned int data_length;   			//lenth of data to be saved
	int split_size;			   				//split size
	char file_save_path[FILEPATHMAX];  		//file save path
	char name[MAX_NAME];                	//name
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
static int ishexstr(char *str)
{
	return (strspn(str, "0123456789ABCDEFabcdef")==strlen(str)); 
}

typedef enum EMDiskSizeType_{
	TOTAL_SIZE,
	FREE_SIZE
} EMDiskSizeType;
typedef enum ERRCODE{	
	NOT_HEX = -3,
	NOT_NUMBER,
	DATA_TOOBIG,
	DATA_TOOSMALL,
	FEW_PARAMETERS,
	FEW_MEMORY,
	PATH_NOTFOUND,
	FILE_ERR,
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
			//llcount =llcount >> 20;  //transfor byte to MB
			break;
		case FREE_SIZE:
			llcount = diskinfo.f_bfree * diskinfo.f_bsize;
			//llcount = llcount >> 20;
			break;		
		default:
			break;
		}		
		return llcount;
	}
}

static int  data_dumped(struct file_property fp, int data_num,long len,int is_single_finish){
	char full_file_path[FILEPATHMAX];
	char full_file_name[MAX_NAME];
	FILE *file = NULL;
	unsigned char *pu8 = NULL;
	char cmd_cmmand[200];
	sprintf(full_file_path,"%s/%s-%d.txt",fp.file_save_path,fp.name,data_num);
	sprintf(full_file_name,"%s/%s-%d",fp.file_save_path,fp.name,data_num);
	
	file = fopen(full_file_path,"a+");  //create the numbered file
	


	if(file == NULL){
		printf("file:%s opened err\n",full_file_path);
		return FILE_ERR;
	}
	fseek(file, 0, SEEK_END);//定位到文件末尾
	pu8 = (unsigned char*)fp.base_addr;
	//printf("file:%s created OK\n",full_file_path);

	fwrite(pu8,sizeof(char),len,file);  //4096 bytes write

	if(is_single_finish){
		printf("split %d write finished!\n",data_num);
		sprintf(cmd_cmmand,"%s %s/%s-%d.tar.gz -C %s/ %s-%d.txt","tar -czPf",fp.file_save_path,fp.name,data_num,
		fp.file_save_path,fp.name,data_num);  //tar -czvf ***.tar ***.bin
		//printf("cmd_cmmand: %s\n",cmd_cmmand);
		system(cmd_cmmand);													   
		sprintf(cmd_cmmand,"%s %s/%s-%d.txt","rm ",fp.file_save_path,fp.name,data_num);   //rm ***.txt
		//printf("cmd_cmmand: %s\n",cmd_cmmand);
		system(cmd_cmmand);	
		
		is_single_finish = 0;
	}
	fclose(file);
	file = NULL;
	return OK;
}
static int data_separated_dump(struct file_property fp){
	unsigned char* data_started = NULL;
	unsigned int data_offset = 0;
	unsigned int split_num;
	unsigned int split_4kb_num;
	long page_size = sysconf(_SC_PAGESIZE);
	int is_single_finish = 0;
	printf("Executed:\n");
	printf("==============>file save   path: %s\n",fp.file_save_path);
	printf("==============>file save length: %d\n",fp.data_length);
	printf("==============>file split  size: %d\n",fp.split_size);
	printf("==============>file split  name: %s\n",fp.name);
	printf("==============>file base   addr: 0x%llx\n",fp.base_addr);


	for(split_num = 0; split_num < fp.split_size; split_num++){
		//write one split ..
		is_single_finish = 0;
		//printf("%ld\n",fp.data_length/fp.split_size/page_size);
		for(split_4kb_num = 1; split_4kb_num <= fp.data_length/fp.split_size/page_size;split_4kb_num++){
			if(split_4kb_num == fp.data_length/fp.split_size/page_size)  //is one split finished?
				is_single_finish = 1;  //last 4Kb flag
			printf("fp.base_addr: %llx\n",fp.base_addr);
			data_dumped(fp,split_num,page_size,is_single_finish);
			fp.base_addr += page_size;    //addr offset 4096Kb
		}
	}
	return OK;
}
static int ap_query_cp_memory(struct file_property fp){
	int fd = -1;
	void *map_addr = NULL;
	if((fp.data_length) >= MAX_DATA_SIZE){
		return DATA_TOOBIG;
	}
	fd = open("/home/chao-zhang/0.txt",O_RDWR);
	if(fd < 0){
		printf("no file/n");
	}
	map_addr = mmap(NULL,201*1024*1024,PROT_READ,MAP_SHARED,fd,0);
	if(map_addr == MAP_FAILED){
		close(fd);
		return -ENOMEM;
	}
	printf("\n\n");
	printf("=====================>file vertual base addr = %llx\n", (unsigned long long )map_addr);
	fp.base_addr += (unsigned long long )map_addr;
	printf("=====================>file vertual base addr + base_addr given = s%llx\n",fp.base_addr);
	printf("\n\n");
	data_separated_dump(fp);
	munmap(map_addr,200*1024*1024);
	close(fd);
}
//path and disksize check
static int parameter_cheak(struct file_property *fp){
	char cuurent_path[FILEPATHMAX];
	DIR *d = NULL;
	unsigned long long per_data_size;   //per data size
	long page_size = sysconf(_SC_PAGESIZE);
	getcwd(cuurent_path,FILEPATHMAX);
	
	if(strcmp(fp->file_save_path,"cl")){
		d = opendir(fp->file_save_path);
		if(d == NULL){
			printf("path not right, default path (current) is used!\n");
			strcpy(fp->file_save_path,cuurent_path);
			closedir(d);
			return PATH_NOTFOUND;
		}
	}else{
		strcpy(fp->file_save_path,cuurent_path);
	}
	unsigned long long disk_space_sount =  get_diskSize(fp->file_save_path,FREE_SIZE);
	per_data_size = fp->data_length/fp->split_size; 
	if((disk_space_sount<=10)||(disk_space_sount < (per_data_size))){
		printf(" err : %s: Lack of space, free: %llu byte\n",fp->file_save_path,disk_space_sount);
		return FEW_MEMORY;
	}

	if(fp->base_addr % page_size != 0){
		printf(" ------------------>addr  must be an intergral muiltiple of 4096byte\n");
		return FILE_ERR;
	}

	if(fp->data_length < fp->split_size * page_size){
		printf("data length :%d is less than split_size*page_size = %ld\n",fp->data_length,fp->split_size * page_size);
		return FILE_ERR;
	} 
	if(fp->data_length % page_size != 0){
		printf("datalength must be an intergral muiltiple of 4096byte\n");
		return FILE_ERR;
	}

	//printf(" %s: Lack of space, free: %llu byte\n",fp->file_save_path,disk_space_sount);
	return OK;
}
static int parse_path_option(char* file_path,struct file_property *fp){
	
	strcpy(fp->file_save_path,file_path);
	return OK;
}
//hex only default
static int parse_data_addr_option(char *data_addr, struct file_property *fp){
	unsigned long long data_base_addr = 0;
	
	if(!ishexstr(data_addr)){
		printf("ERR: -a: HEX ONLY\n");
		return NOT_NUMBER;
	}
	sscanf(data_addr,"%llx",&data_base_addr); 
	printf("---------------------------> addr hex: %llx  dec: %lld\n",data_base_addr,data_base_addr);

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

	struct file_property fp_set;
	fp_set = fileproperty;
	int ret = 0;
	int cmd = 0;
	if(argc<2){	
		printf("Usage:\n");
		printf("-a:base addrress (HEX) -l: data lenth\n");
		printf("[-d:location] [-s:split number] [-n:name] \n");
		return FEW_PARAMETERS;
	}	
	//printf("pid : %d\n",getpid());
	while((cmd = getopt(argc, argv,"a:l:d:s:n:"))!=-1){  //getopt will retrn -1 when all the parameters are listed
		switch (cmd)
		{
		case 'a':
			if(optarg){
				ret = parse_data_addr_option(optarg,&fp_set);
				if(ret != OK){
					exit(6);
				}
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
	//option parameter check path and disk size check
	if(parameter_cheak(&fp_set)!=OK) return -1;
	fileproperty = fp_set;

	printf("Entered:\n");
	printf("==============>file save   path: %s\n",fileproperty.file_save_path);
	printf("==============>file save length: %d\n",fileproperty.data_length);
	printf("==============>file split  size: %d\n",fileproperty.split_size);
	printf("==============>file split  name: %s\n",fileproperty.name);
	printf("==============>file base   addr: 0x%llx\n",fileproperty.base_addr);

	ap_query_cp_memory(fileproperty);
	
	
	/*
	//data created!
	struct file_property fp;
	char cuurent_path[FILEPATHMAX];
	getcwd(cuurent_path,FILEPATHMAX);
	memset(&fp,0,sizeof(struct file_property));
	const unsigned int total_size = 1024*1024*200;    //100MB
	//char data[total_size];
	char *data=malloc(1024*1024*200);
	for(int i = 0; i<total_size; i++){
		data[i] = '1';
	}
	strcpy(fp.file_save_path,cuurent_path);
	fp.base_addr = (unsigned long long)data;
	data_dumped(fp,0,total_size,0);
	*/
	
}
