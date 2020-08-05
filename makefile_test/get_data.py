# -*- coding: utf-8 -*-   
import tarfile
import os
import sys
import bz2
import re
import getopt
VERSION = "arm-0.3.1"

name_format = ".tar.bz2" #"tar.bz2" 
def files_name_get(file_dir,name):   
    target_file=[]   
    name_str = ""
    if name:
        name_str = name              
    has_tar_file = False    #没输入名字会遍历目录 找到存在的文件
    for files in os.listdir(file_dir):  
        if ((name_str in files) and (name_format in files)):   #ensure "name" and "name_format" are included in the file
            #print("name:"+name+"file+"+files)
            file_ = files.split('-',2)
            target_file.append( file_[0]+'-'+file_[1])  #get data string name
            has_tar_file = True
    if not has_tar_file:
        print("path :%s has no dumped file-----> end up with %s"%(file_dir,name_format))
        exit(0)
    return list(set(target_file))   #去重返回
def file_extract(sorted_file,file_source_dir,target_name,file_target_dir):
    for i in range(len(sorted_file)):
        for files_ in os.listdir(file_source_dir):
            if ((sorted_file[i] in files_) and (name_format in files_)):
                data_to_extrat = os.path.join(file_source_dir,files_)
                #print(data_to_extrat)  
                try:
                    tar = tarfile.open(data_to_extrat,"r:*",encoding='utf-8')
                except IOError as e:
                    print(e)
                    exit(1)
                tar.extractall(path=file_target_dir)
                file_target_path = os.path.join(file_target_dir,tar.getnames()[0])
                tar.close()
                file_data_save = os.path.join(file_target_dir,target_name+'.bin')
                #open file and write files at the end  
                #参考：https://blog.51cto.com/14320361/2486142

                with open(file_data_save,'ab') as f:   #以二进制格式打开一个文件，改文件为追加模式
                    f.write(open(file_target_path,'rb').read())  #以二进制格式读取一个文件，并将其写入到目标文件夹 
                os.remove(file_target_path)
def file_save(files_names,file_source_dir,file_target_dir): #get different files
    for i in range(len(files_names)):
        sorted_file = file_splitnumber_sort(files_names[i],file_source_dir)  #sorted the specific name "e.g. data-0"
        #print(sorted_file)
        file_extract(sorted_file,file_source_dir,files_names[i],file_target_dir)
    return 0
def get_file_dir(file_dir_opt):
    if file_dir_opt:
        #print(file_dir_opt)
        if not (os.path.isdir(file_dir_opt) or (os.path.exists(file_dir_opt))):
            print("you entered: %s cannot find!"%file_dir_opt)
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
        if ((file_name in file) and (name_format in file)):
            file_split_num = file.split('.')[0]
            file_list.append(file_split_num)
    file_list.sort(key=sort_key)
    file_sorted_list = file_list
    return file_sorted_list

def main():
    file_path_source_entered = ""
    file_path_dest_entered = ""
    file_name_entered = ""
    try:
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
    except Exception as e:
        print(e)
        exit(1)
    file_source_dir = get_file_dir(file_path_source_entered)   #获得路径
    file_target_dir = get_file_dir(file_path_dest_entered)
    #print("file_source_dir"+file_source_dir)
    #print("file_target_dir"+file_target_dir)
    files_names = files_name_get(file_source_dir,file_name_entered)   #获得当前目录下去重的文件名
    file_save(files_names,file_source_dir,file_target_dir)              
    print("OK!")
    return 0
if __name__ == "__main__":
    main()
