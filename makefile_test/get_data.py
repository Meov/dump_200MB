# -*- coding: utf-8 -*-   
import tarfile
import os
import sys
import bz2
import re
import getopt



def files_name_get(file_dir,name):   
    target_file=[]   
    name_str = ""
    name_format = ".tar" #"tar.bz2" 
    if name:
        name_str = name              
    has_tar_file = False    #没输入名字会遍历目录 找到存在的文件
    for root, dirs, files in os.walk(file_dir):  
        for file_ in files:
            if ((name_str in file_) and (name_format in file_)):   #ensure "name" and "name_format" are included in the file
                #print("name:"+name+"file+"+file_)
                file_ = file_.split('-',2)
                target_file.append( file_[0]+'-'+file_[1])  #get data string name
                has_tar_file = True
    if not has_tar_file:
        print("path :%s has no dumped file-----> end up with -tar.bz2"%file_dir)
        exit(0)

    return list(set(target_file))   #去重返回

def create_file(file_path,msg):
    f=open(file_path,"a")
    f.write(msg)
    f.close

def file_extract(sorted_file,file_source_dir,file_target_dir):
    #create_file(file_path_dest+".bin","hello") 
    
    for i in range(len(sorted_file)):
        for files_ in os.listdir(file_source_dir):
            #print(file_source_dir)
            if sorted_file[i] in files_:
                
                data_to_extrat = os.path.join(file_source_dir,files_)
                print(data_to_extrat)
                tar = tarfile.open(data_to_extrat,"r:*",encoding='utf-8')
                tar.extractall()
"""
                tar = tarfile.open(files_,"r:*",encoding='utf-8')
                names = tar.getnames()
                for name in names:
                    print(name)
                    tar.extract(name,path = file_target_dir)
                tar.close()
                """
def file_save(files_names,file_source_dir): #get different files
    for i in range(len(files_names)):
        sorted_file = file_splitnumber_sort(files_names[i],file_source_dir)  #将某一name下按照split_number重新排列
        print(sorted_file)
        file_extract(sorted_file,file_source_dir,"s")
    return 0
def get_file_dir(file_dir_opt):
    if file_dir_opt:
        #print(file_dir_opt)
        if not (os.path.isdir(file_dir_opt) or (os.path.exists(file_dir_opt))):
            print("you entered: %s is not a directory!"%file_dir_opt)
            exit(0)
        else:
            file_dir = file_dir_opt
    else:
        file_dir = os.path.split(os.path.realpath(__file__))[0]#current path defualt
    return file_dir
def sort_key(s):
    if s:
        try:
            suffix = re.search('\\d+$', s)
            num = int(suffix.group())
        except:
            num = -1
        return num
def file_splitnumber_sort(file_name,path):    #将同一个文件（name-dumpednumer）按照不同的split number进行排序
    print("searching for ...  "+file_name+" in %s"%path)
    file_list = []
    for file in os.listdir(path):
        if file_name in file:
            #print(file)
            file_split_num = file.split('.')[0]
            file_list.append(file_split_num)
    file_list.sort(key=sort_key)
    file_sorted_list = file_list
    return file_sorted_list

def main():

    file_path_source_entered = ""
    file_path_dest_entered = ""
    file_name_entered = ""
    opts, _ = getopt.getopt(sys.argv[1:],'d:n:t:',["path","name","target-path"])
    for opt,value in opts:
        if opt in ("-d","--path"):
            file_path_source_entered = value
            print(file_path_source_entered)
        if opt in ("-n","name"):    #这里跟C语言不一样...  
            file_name_entered = value
            print(file_name_entered)
        if opt in ("-t","--target-path"):
            file_path_dest_entered = value
            print(file_path_dest_entered)
        pass
   
    file_source_dir = get_file_dir(file_path_source_entered)   #获得路径
    file_target_dir = get_file_dir(file_path_dest_entered)
    
    print("file_source_dir"+file_source_dir)
    print("file_target_dir"+file_target_dir)
    
    files_names = files_name_get(file_source_dir,file_name_entered)   #获得当前目录下去重的文件名
    #print(files_name)
    file_save(files_names,file_source_dir)              

    return 0
if __name__ == "__main__":
    main()