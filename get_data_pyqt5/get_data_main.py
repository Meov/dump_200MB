import sys
from PyQt5.QtWidgets import QApplication, QMainWindow
from get_data_action import MainWindow
if __name__ == '__main__':
    app = QApplication(sys.argv)
    MainWindow = MainWindow()
    MainWindow.show()
    sys.exit(app.exec_())
