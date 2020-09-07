# -*- coding: utf-8 -*-   
import tarfile
import os
import sys
import bz2
import re
import getopt

name_format = ".tar.bz2" #"tar.bz2" 
class BinCreat(object):

    def __init__(self,parent=None):
        self.file_path_source_entered = ""
        self.file_path_dest_entered = ""
        self.file_name_entered = ""

    def get_file_dir(self,file_dir_opt):
        if file_dir_opt:
            #print(file_dir_opt)
            if not (os.path.isdir(file_dir_opt) or (os.path.exists(file_dir_opt))):
                print("you entered: %s cannot find!"%file_dir_opt)
                #exit(0)
            else:
                file_dir = file_dir_opt
        else:
            file_dir = os.path.split(os.path.realpath(__file__))[0]#current path defualt
        return file_dir
    
    def files_name_get(self,file_dir,name):   
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
            #exit(0)
        return list(set(target_file))   #去重返回
        
    def sort_key(self,s):
        if s:
            try:
                suffix = re.search('\\d+$', s)
                num = int(suffix.group())
            except:
                num = -1
            return num
 
    def file_extract(self,file_source_dir,target_name,file_target_dir):
        for files_ in os.listdir(file_source_dir):
            if ((str(target_name) in files_) and (name_format in files_)):
                data_to_extrat = os.path.join(file_source_dir,files_)
                print(data_to_extrat)  
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

    def binary_file_name_get(self,file_path_source_entered,file_path_dest_entered):
        self.file_path_source_entered = file_path_source_entered 
        self.file_path_dest_entered = file_path_dest_entered
        files_names = self.files_name_get(self.file_path_dest_entered,self.file_name_entered)   #获得当前目录下去重的文件名
        return files_names

    def binary_file_create(self,file_source_dir,file_path_dest,file_name): 
        self.file_extract(file_source_dir,file_name,file_path_dest)
        