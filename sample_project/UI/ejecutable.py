import sys
import os
from PyQt5 import QtCore, Qt, uic, QtWidgets
from pyqtgraph import QtGui, PlotWidget
from interfaz import Ui_Dialog
from PyQt5.QtGui import QPixmap
from PyQt5.QtCore import QThread
from PyQt5.QtWidgets import (
    QApplication,
    QMainWindow,
    QDialog,
    QPushButton,
    QLabel,
    QMessageBox,
    QWidget,
    QInputDialog,
    QSlider,
    QLineEdit,
    QGraphicsView,
)
from threading import Thread
import serial
import pyqtgraph
import numpy
import os.path as path
import time
import scipy.io
#import pygame
from serialtest import *



class SerialThread(QThread):
    def __init__(self):
        super().__init__()
        self.running = True
        self.data = []

    def stop(self):
        self.running = False
        send_end_message()

    def plot1(self,cont):
        p = cont.plot_1
        # Crear los datos de tiempo y temperatura
        tiempo = [0, 1, 2, 3, 4, 5, 6, 7, 8]
        
        data = self.data
        print("uwu: ",data)
        #temperatura = [25, 24, 23, 22, 21, 20, 19, 18, 17, 16]

        # Agregar el gráfico de tiempo vs temperatura al objeto plot_1
        p.plot(tiempo, data, pen='b', symbol='o')

        # Configurar el eje x y el eje y con etiquetas
        p.setLabel('left', 'Temperatura (°C)')
        p.setLabel('bottom', 'Tiempo (s)')

    def run(self):
        try:
   
            data_generator = begin_serial()
            for data in data_generator:
                print("data for",data)
                self.data = list(data)
                self.plot1(data,cont)
                print("data=",self.data)
            #print(self.data)
        except serial.SerialException as e:
            print(f"Error en la conexión serial: {str(e)}")


class MainWindow():

    def __init__(self, parent):
        #super(MainWindow, self).init()
        self.ui = Ui_Dialog()
        self.parent = parent
        self.serialThread = SerialThread()
        #self.serialRead.readSerialStart()
        

    def stopSerialRead(self):
        print("stop")
        self.serialThread.stop()
        self.serialThread.wait()  # Esperar a que el hilo termine antes de continuar

    def setSignals(self):
        self.ui.selec_12.currentIndexChanged.connect(self.leerModoOperacion)
        self.ui.pushButton.clicked.connect(self.leerModoOperacion)
        self.ui.test_boton.clicked.connect(self.empezarSerialRead)
        self.ui.test_boton_2.clicked.connect(self.stopSerialRead)

    def empezarSerialRead(self):
        self.serialThread.start()
        self.serialThread.plot1(self.ui)

    def stopSerialRead(self):
        self.serialThread.stop()
        

    def leerConfiguracionAcelerometro(self):
        conf = dict()
        conf['AccSamp'] = self.ui.text_acc_sampling.toPlainText()
        conf['AccSen'] = self.ui.text_acc_sensibity.toPlainText()
        print(conf)
        return conf
    
    def leerConfiguracionGiroscopio(self):
        conf = dict()
        conf['AccSamp'] = self.ui.text_acc_sampling_2.toPlainText()
        conf['AccSen'] = self.ui.text_acc_sensibity_2.toPlainText()
        print(conf)
        return conf

    def leerModoOperacion(self):
        index = self.ui.selec_12.currentIndex()
        texto =self.ui.selec_12.itemText(index)
        print(texto)
        return texto
    

if __name__ == "__main__":
    import sys
    app= QtWidgets.QApplication(sys.argv)

    Dialog = QtWidgets.QDialog()
    cont = MainWindow(parent=Dialog)
    ui = cont.ui
    ui.setupUi(Dialog)
    Dialog.show()
    cont.setSignals()
    #cont.plot1()
    sys.exit(app.exec_())
