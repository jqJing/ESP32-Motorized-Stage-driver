# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file '1.ui'
##
## Created by: Qt User Interface Compiler version 6.4.2
##
## WARNING! All changes made in this file will be lost when recompiling UI file!
################################################################################

from PySide6.QtCore import (QCoreApplication, QDate, QDateTime, QLocale,
    QMetaObject, QObject, QPoint, QRect,
    QSize, QTime, QUrl, Qt)
from PySide6.QtGui import (QBrush, QColor, QConicalGradient, QCursor,
    QFont, QFontDatabase, QGradient, QIcon,
    QImage, QKeySequence, QLinearGradient, QPainter,
    QPalette, QPixmap, QRadialGradient, QTransform)
from PySide6.QtWidgets import (QApplication, QComboBox, QHBoxLayout, QLabel,
    QLineEdit, QListWidget, QListWidgetItem, QMainWindow,
    QPushButton, QSizePolicy, QStatusBar, QWidget)

class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        if not MainWindow.objectName():
            MainWindow.setObjectName(u"MainWindow")
        MainWindow.resize(705, 361)
        self.centralwidget = QWidget(MainWindow)
        self.centralwidget.setObjectName(u"centralwidget")
        self.label_3 = QLabel(self.centralwidget)
        self.label_3.setObjectName(u"label_3")
        self.label_3.setGeometry(QRect(10, 180, 101, 29))
        font = QFont()
        font.setPointSize(14)
        font.setBold(False)
        self.label_3.setFont(font)
        self.label_4 = QLabel(self.centralwidget)
        self.label_4.setObjectName(u"label_4")
        self.label_4.setGeometry(QRect(10, 10, 101, 29))
        self.label_4.setFont(font)
        self.label_7 = QLabel(self.centralwidget)
        self.label_7.setObjectName(u"label_7")
        self.label_7.setGeometry(QRect(190, 250, 21, 29))
        self.label_7.setFont(font)
        self.label_9 = QLabel(self.centralwidget)
        self.label_9.setObjectName(u"label_9")
        self.label_9.setGeometry(QRect(10, 90, 101, 29))
        self.label_9.setFont(font)
        self.dir1btn = QPushButton(self.centralwidget)
        self.dir1btn.setObjectName(u"dir1btn")
        self.dir1btn.setGeometry(QRect(20, 120, 91, 41))
        font1 = QFont()
        font1.setPointSize(12)
        font1.setBold(True)
        self.dir1btn.setFont(font1)
        self.dir2btn = QPushButton(self.centralwidget)
        self.dir2btn.setObjectName(u"dir2btn")
        self.dir2btn.setGeometry(QRect(120, 120, 91, 41))
        self.dir2btn.setFont(font1)
        self.outputbtn = QPushButton(self.centralwidget)
        self.outputbtn.setObjectName(u"outputbtn")
        self.outputbtn.setGeometry(QRect(20, 290, 311, 51))
        font2 = QFont()
        font2.setPointSize(14)
        font2.setBold(True)
        self.outputbtn.setFont(font2)
        self.label_10 = QLabel(self.centralwidget)
        self.label_10.setObjectName(u"label_10")
        self.label_10.setGeometry(QRect(200, 190, 141, 51))
        self.label_10.setScaledContents(False)
        self.label_10.setWordWrap(True)
        self.lstwidget = QListWidget(self.centralwidget)
        self.lstwidget.setObjectName(u"lstwidget")
        self.lstwidget.setGeometry(QRect(350, 10, 341, 331))
        self.layoutWidget = QWidget(self.centralwidget)
        self.layoutWidget.setObjectName(u"layoutWidget")
        self.layoutWidget.setGeometry(QRect(20, 40, 111, 31))
        self.layout13 = QHBoxLayout(self.layoutWidget)
        self.layout13.setObjectName(u"layout13")
        self.layout13.setContentsMargins(0, 0, 0, 0)
        self.label = QLabel(self.layoutWidget)
        self.label.setObjectName(u"label")
        self.label.setFont(font1)

        self.layout13.addWidget(self.label)

        self.portbox = QComboBox(self.layoutWidget)
        self.portbox.addItem("")
        self.portbox.addItem("")
        self.portbox.addItem("")
        self.portbox.addItem("")
        self.portbox.addItem("")
        self.portbox.addItem("")
        self.portbox.addItem("")
        self.portbox.setObjectName(u"portbox")

        self.layout13.addWidget(self.portbox)

        self.layoutWidget1 = QWidget(self.centralwidget)
        self.layoutWidget1.setObjectName(u"layoutWidget1")
        self.layoutWidget1.setGeometry(QRect(140, 40, 111, 31))
        self.layout23 = QHBoxLayout(self.layoutWidget1)
        self.layout23.setObjectName(u"layout23")
        self.layout23.setContentsMargins(0, 0, 0, 0)
        self.label_2 = QLabel(self.layoutWidget1)
        self.label_2.setObjectName(u"label_2")
        self.label_2.setFont(font1)

        self.layout23.addWidget(self.label_2)

        self.baudbox = QComboBox(self.layoutWidget1)
        self.baudbox.addItem("")
        self.baudbox.setObjectName(u"baudbox")

        self.layout23.addWidget(self.baudbox)

        self.layoutWidget2 = QWidget(self.centralwidget)
        self.layoutWidget2.setObjectName(u"layoutWidget2")
        self.layoutWidget2.setGeometry(QRect(20, 250, 161, 31))
        self.horizontalLayout = QHBoxLayout(self.layoutWidget2)
        self.horizontalLayout.setObjectName(u"horizontalLayout")
        self.horizontalLayout.setContentsMargins(0, 0, 0, 0)
        self.label_6 = QLabel(self.layoutWidget2)
        self.label_6.setObjectName(u"label_6")
        self.label_6.setFont(font1)

        self.horizontalLayout.addWidget(self.label_6)

        self.number = QLineEdit(self.layoutWidget2)
        self.number.setObjectName(u"number")

        self.horizontalLayout.addWidget(self.number)

        self.layoutWidget3 = QWidget(self.centralwidget)
        self.layoutWidget3.setObjectName(u"layoutWidget3")
        self.layoutWidget3.setGeometry(QRect(20, 210, 161, 31))
        self.horizontalLayout_23 = QHBoxLayout(self.layoutWidget3)
        self.horizontalLayout_23.setObjectName(u"horizontalLayout_23")
        self.horizontalLayout_23.setContentsMargins(0, 0, 0, 0)
        self.label_5 = QLabel(self.layoutWidget3)
        self.label_5.setObjectName(u"label_5")
        self.label_5.setFont(font1)

        self.horizontalLayout_23.addWidget(self.label_5)

        self.freq = QLineEdit(self.layoutWidget3)
        self.freq.setObjectName(u"freq")

        self.horizontalLayout_23.addWidget(self.freq)

        self.layoutWidget4 = QWidget(self.centralwidget)
        self.layoutWidget4.setObjectName(u"layoutWidget4")
        self.layoutWidget4.setGeometry(QRect(210, 250, 121, 31))
        self.horizontalLayout_33 = QHBoxLayout(self.layoutWidget4)
        self.horizontalLayout_33.setObjectName(u"horizontalLayout_33")
        self.horizontalLayout_33.setContentsMargins(0, 0, 0, 0)
        self.label_8 = QLabel(self.layoutWidget4)
        self.label_8.setObjectName(u"label_8")
        self.label_8.setFont(font1)

        self.horizontalLayout_33.addWidget(self.label_8)

        self.coeff = QLineEdit(self.layoutWidget4)
        self.coeff.setObjectName(u"coeff")

        self.horizontalLayout_33.addWidget(self.coeff)

        MainWindow.setCentralWidget(self.centralwidget)
        self.statusbar = QStatusBar(MainWindow)
        self.statusbar.setObjectName(u"statusbar")
        MainWindow.setStatusBar(self.statusbar)

        self.retranslateUi(MainWindow)

        QMetaObject.connectSlotsByName(MainWindow)
    # setupUi

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(QCoreApplication.translate("MainWindow", u"MainWindow", None))
        self.label_3.setText(QCoreApplication.translate("MainWindow", u"PWM\u8f93\u51fa\uff1a", None))
        self.label_4.setText(QCoreApplication.translate("MainWindow", u"\u4e32\u53e3\u8bbe\u7f6e\uff1a", None))
        self.label_7.setText(QCoreApplication.translate("MainWindow", u"\u00d7", None))
        self.label_9.setText(QCoreApplication.translate("MainWindow", u"\u79fb\u52a8\u65b9\u5411\uff1a", None))
        self.dir1btn.setText(QCoreApplication.translate("MainWindow", u"\u65b9\u54111", None))
        self.dir2btn.setText(QCoreApplication.translate("MainWindow", u"\u65b9\u54112", None))
        self.outputbtn.setText(QCoreApplication.translate("MainWindow", u"PWM\u8f93\u51fa", None))
        self.label_10.setText(QCoreApplication.translate("MainWindow", u"\u5b9e\u9645\u8f93\u51fa\u6570\u91cf\u7b49\u4e8e\u65b9\u6ce2\u6570\u91cf\u4e58\u4ee5\u7cfb\u6570\u3002\u901a\u8fc7\u8fd9\u79cd\u65b9\u5f0f\u8bbe\u5b9a\u6ed1\u53f0\u7684\u4e0d\u540c\u7ec6\u5206", None))
        self.label.setText(QCoreApplication.translate("MainWindow", u"\u7aef\u53e3", None))
        self.portbox.setItemText(0, QCoreApplication.translate("MainWindow", u"COM3", None))
        self.portbox.setItemText(1, QCoreApplication.translate("MainWindow", u"COM4", None))
        self.portbox.setItemText(2, QCoreApplication.translate("MainWindow", u"COM5", None))
        self.portbox.setItemText(3, QCoreApplication.translate("MainWindow", u"COM6", None))
        self.portbox.setItemText(4, QCoreApplication.translate("MainWindow", u"COM7", None))
        self.portbox.setItemText(5, QCoreApplication.translate("MainWindow", u"COM8", None))
        self.portbox.setItemText(6, QCoreApplication.translate("MainWindow", u"COM9", None))

        self.label_2.setText(QCoreApplication.translate("MainWindow", u"\u6ce2\u7279\u7387", None))
        self.baudbox.setItemText(0, QCoreApplication.translate("MainWindow", u"9600", None))

        self.label_6.setText(QCoreApplication.translate("MainWindow", u"\u65b9\u6ce2\u6570\u91cf\uff1a", None))
        self.label_5.setText(QCoreApplication.translate("MainWindow", u"\u9891\u7387(Hz)\uff1a", None))
        self.label_8.setText(QCoreApplication.translate("MainWindow", u"\u7cfb\u6570\uff1a", None))
    # retranslateUi

