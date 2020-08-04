# -*- coding: utf-8 -*-   
import tarfile
import os
import sys
import bz2

class file_property:
    name = "",
    dump_num = 0,
    split_num = 1,
def files_name_get(file_dir):   
    target_file=[]   
    has_tar_file = False
    for root, dirs, files in os.walk(file_dir):  
        for file in files:  
            #if "tar.bz2" in file:
            if ".tar" in file:
                file = file.split('-',2)
                target_file.append( file[0]+'-'+file[1])  #get data string name
                has_tar_file = True
    if not has_tar_file:
        print("path :%s has no dumped file-----> end up with -tar.bz2"%file_dir)
        exit()
    return list(set(target_file))

def create_file(file_path,msg):
    f=open(file_path,"a")
    f.write(msg)
    f.close
def file_merge(file_path_dest,file_name_source):
    #create_file(file_path_dest+".bin","hello") 
    tar = tarfile.open(file_name_source,"r:*",encoding='utf-8')
    names = tar.getnames()
    for name in names:
        print(name)
        tar.extract(name,path = file_path_dest)
    tar.close()

def files_merge(files_name,path): #get different files
    for i in range(len(files_name)):
        file_savenumber_get(files_name[i],path) 
    return 0

def get_file_dir(file_dir_opt):
    #get current file path or user inpur
    if(len(file_dir_opt)== 1):
        file_dir = os.getcwd() #current path defualt
    else:
        file_dir = file_dir_opt[1]
        if not os.path.exists(file_dir):
            print("path you entered dosen't exsist!")
            exit()   #path err!
    print("path is :",format(file_dir))
    return file_dir

def file_savenumber_get(file_name,path):
    print("searching for ...  "+file_name)
    #这已经是同一个文件了
    L = []
    for file in os.listdir(path):
        if file_name in file: 
            file = file.split('.',1)
            file = file[0].split('-',2)
            print(file[-1])
    
    
    """
    for root, dirs, files in os.walk(path):
        for file in files:
            if file_name in file:
                print(file)
if(len(file_dir_opt)== 1):
        file_dir = os.path.split(os.path.realpath(__file__))[0]#current path defualt
    return file_dir

