# encoding: utf-8
import zipfile
import tarfile
import os
import sys


file_path = "current path"
file_result = []
files_list = []

def list_files(tar_file_str):
    if file_path:
        i = 0
        for dirpath, dirnames, filenames in os.walk(file_path):
            for file in filenames:
                if tar_file_str in file:
                    file_result = [os.path.join(dirpath,file)]
                    i += 1
                    j = [i]
                    files_list.append(file_result)
                    print(j + file_result)
    else:
        print ("The dirpath in not Exist!")


def find_files():
    filename = input("enter the file name you want:")
    full_file_path = os.path.join(file_path,filename)
    for finefull_file_path_str in files_list:
        print(finefull_file_path_str)
        if finefull_file_path_str[0].find(full_file_path,0,len(finefull_file_path_str[0])) != -1:
            print(finefull_file_path_str[0])



    print(full_file_path)
    print(0)

def merge_files():
    t = tarfile.open('data-0-0.tar.bz2', 'r')
    t.extractall()
    t.close()

def get_files_path():
    if(len(sys.argv)<2):
        path = os.getcwd()
        print("no inputs ---- defualt location:%s",path)
    else:
        path = sys.argv[1]
    #print(path)

    file_path = path


def main():
    get_files_path()
    list_files(".tar.bz2")
    find_files()
    merge_files()


if __name__ == '__main__':
    main()
  
