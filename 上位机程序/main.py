import sys
from PySide6 import QtCore
from PySide6.QtWidgets import QApplication,QFileDialog,QTableWidget,QTableWidgetItem, QListWidget,QListWidgetItem
from PySide6.QtGui import QColor
from PySide6.QtCore import QSize
from PySide6.QtNetwork import QUdpSocket
from PySide6.QtNetwork import QHostAddress
import pyqtgraph as pg
from PySide6.QtCore import Qt
import os
import json
import time
import numpy as np
import serial
from datetime import datetime

uiclass,baseclass = pg.Qt.loadUiType("1.ui")

class MainWindow(uiclass,baseclass):
    def __init__(self):
        super().__init__()
        self.setupUi(self)
        self.message_count = 0
        #initialization
        self.freq.setText("1000")
        self.coeff.setText("80")
        #SignalBinding
        self.outputbtn.clicked.connect(self.pwm_output)
        self.dir1btn.clicked.connect(self.set_dir_1)
        self.dir2btn.clicked.connect(self.set_dir_2)
        self.testbtn.clicked.connect(self.test)


    def test(self):
        self.print_message("Hello")

    def set_dir_1(self):
        self.send_msg("12")

    def set_dir_2(self):
        self.send_msg("13")

    def pwm_output(self):
        # 先频率 后数量
        frequence = int(self.freq.text())
        pulse_number = int(self.number.text())
        coefficient = int(self.coeff.text())
        order = "11,"+str(frequence) + ","+str(pulse_number*coefficient)

        self.print_message("发送指令-> "+order)
        self.send_msg(order)

    def send_msg(self,ss):
        serialport = self.portbox.currentText()
        baudrate = int(self.baudbox.currentText())
        if not ss.endswith("\n"):
            ss = ss + "\n"

        try:
            with serial.Serial(serialport,baudrate,timeout=1) as ser:
                ser.write(ss.encode('utf-8'))

            self.print_message("成功发送一条指令： "+ss)
        except:
            self.print_message("发送指令失败： "+ss)

    def print_message(self,message):
        self.message_count += 1

        current_time = datetime.now().strftime('%Y-%m-%d %H:%M:%S')  # 获取当前时间并格式化
        numbered_message = f"{self.message_count}. {message} [{current_time}]"  # 创建带序号和时间戳的消息
        item = QListWidgetItem(numbered_message)
        if self.message_count % 3 == 1:
            item.setBackground(QColor('lightblue'))
        elif self.message_count % 3 == 2:
            item.setBackground(QColor('lightgreen'))
        else:
            item.setBackground(QColor('lightyellow'))
        self.lstwidget.addItem(item)
        self.lstwidget.scrollToBottom()





if __name__ == '__main__':
    app = QApplication(sys.argv)
    app.setStyle('Fusion')
    w = MainWindow()
    w.show()
    app.exec()