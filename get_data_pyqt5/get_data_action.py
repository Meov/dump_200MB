import sys
import os
from PyQt5.QtWidgets import QMainWindow
from PyQt5.QtCore import QFileInfo
from PyQt5.QtWidgets import QFileDialog
from get_data_ui import *
from bin_creat import BinCreat
bincreat = BinCreat()
class MainWindow(QMainWindow,Ui_MainWindow):
    def __init__(self,parent=None):
        super(MainWindow,self).__init__(parent)
        self.setupUi(self)
        self.dump_browse_Button.clicked.connect(self.open_split_file_dir)
        self.dump_get_Button.clicked.connect(self.getDumpfile)
        self.dump_save_Button.clicked.connect(self.set_save_dump_dir)
        self.dump_exit_app_Button.clicked.connect(self.dump_app_exit)
        self.open_bin_dir_Button.clicked.connect(self.open_dunp_bin_dir)
        self.dump_split_file_dir = '.'
        self.dump_bin_save_dir = '.'

    def print_dump_out_txt(self,toText):
        self.dump_output_txt.append(toText)
        self.cursor=self.dump_output_txt.textCursor()
        self.dump_output_txt.moveCursor(self.cursor.End)
        # was suggested for smooth showing
        QtWidgets.QApplication.processEvents()
    
    def open_split_file_dir(self):
        self.dump_path = QFileDialog.getExistingDirectory(self, "请选择dump文件路径", ".")
        self.dump_path = self.dump_path.replace('/','\\')

        self.dump_path_line.setText(str(self.dump_path))
        self.dump_split_file_dir = self.dump_path
        names = bincreat.binary_file_name_get(self.dump_split_file_dir,self.dump_split_file_dir)
        self.dump_output_txt.clear()
        if names:
            self.combo_dump_name.addItems(names)
        else:
            self.print_dump_out_txt(self.dump_split_file_dir + ": has no dumped files!")
    
    def set_save_dump_dir(self):
        self.save_path = QFileDialog.getExistingDirectory(self, "请选择保存路径", ".")
        self.save_path = self.save_path.replace('/','\\')
        self.dump_save_line.setText(str(self.save_path))
        self.dump_bin_save_dir = self.save_path
    
    def getDumpfile(self):
        
        self.dump_split_file_dir = bincreat.get_file_dir(self.dump_split_file_dir)
        self.dump_bin_save_dir = bincreat.get_file_dir(self.dump_bin_save_dir)

        if (os.path.isdir(self.dump_split_file_dir) and (os.path.isdir(self.dump_bin_save_dir))):
            file_name = self.combo_dump_name.currentText()
            if(file_name):
                self.print_dump_out_txt("extrating data "+str(file_name)+"...")
                bincreat.binary_file_create(self.dump_split_file_dir,self.dump_bin_save_dir,file_name)
                self.print_dump_out_txt("OK! " + file_name+".bin"+ " saved in: "+os.path.join( self.dump_bin_save_dir,file_name))
            else:
                self.print_dump_out_txt("could not find dumped files!")
        else:
            self.print_dump_out_txt("path not right!")

    def open_dunp_bin_dir(self):
        self.dump_bin_save_dir = bincreat.get_file_dir(self.dump_bin_save_dir)  
        if os.path.isdir(self.dump_bin_save_dir):
            os.system("start explorer " + self.dump_bin_save_dir)
        else:
            self.print_dump_out_txt("path not right!")

    def dump_app_exit(self):
        exit(0)

