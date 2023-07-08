import sys
import os
from PyQt5 import QtCore, Qt, uic, QtWidgets
from pyqtgraph import QtGui, PlotWidget
from interfaz import Ui_Dialog
from PyQt5.QtGui import QPixmap
from PyQt5.QtCore import QThread,QTimer
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
import numpy
import os.path as path
import time
import scipy.io
from serialtest import *
from collections import deque


class SerialThread(QThread):
    def __init__(self):
        super().__init__()
        self.running = True
        self.data = []

        self.acc_x = deque(maxlen=10)
        self.acc_y = deque(maxlen=10)
        self.acc_z = deque(maxlen=10)

        self.gyr_x = deque(maxlen=10)
        self.gyr_y = deque(maxlen=10)
        self.gyr_z = deque(maxlen=10)

        self.acc_xg = deque(maxlen=10)
        self.acc_yg = deque(maxlen=10)
        self.acc_zg = deque(maxlen=10)

        self.tiempo = deque(maxlen=10)
        self.count_time = 0

    def stop(self):
        self.running = False
        send_end_message()

    def plot1(self,cont):
        p = cont.plot_1
        p.clear()

        # Crear los datos de tiempo y temperatura
        #tiempo = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
        p.setYRange(-50, 50, padding=0)

        data = self.data
        print("uwu: ",data)
        #temperatura = [25, 24, 23, 22, 21, 20, 19, 18, 17, 16]

        # Agregar el gráfico de tiempo vs temperatura al objeto plot_1
        line1 = p.plot(list(self.tiempo), list(self.acc_x), pen='b', symbol='o')
        line2 = p.plot(list(self.tiempo), list(self.acc_y), pen='r', symbol='o')
        line3 = p.plot(list(self.tiempo), list(self.acc_z), pen='g', symbol='o')
        
        # Configurar el eje x y el eje y con etiquetas
        p.setLabel('left', 'Temperatura (°C)')
        p.setLabel('bottom', 'Tiempo (s)')

    def plot2(self,cont):
        p2 = cont.plot_2
        p2.clear()
        p2.setYRange(-5, 5, padding=0)

        # Agregar el gráfico de tiempo vs temperatura al objeto plot_1
        line1 = p2.plot(list(self.tiempo), list(self.gyr_x), pen='b', symbol='o')
        line2 = p2.plot(list(self.tiempo), list(self.gyr_y), pen='r', symbol='o')
        line3 = p2.plot(list(self.tiempo), list(self.gyr_z), pen='g', symbol='o')
        
        # Configurar el eje x y el eje y con etiquetas
        p2.setLabel('left', 'Temperatura (°C)')
        p2.setLabel('bottom', 'Tiempo (s)')

    def plot3(self,cont):
        p3 = cont.plot_3
        p3.clear()
        p3.setYRange(-10, 10, padding=0)
        # Agregar el gráfico de tiempo vs temperatura al objeto plot_1
        line1 = p3.plot(list(self.tiempo), list(self.acc_xg), pen='b', symbol='o')
        line2 = p3.plot(list(self.tiempo), list(self.acc_yg), pen='r', symbol='o')
        line3 = p3.plot(list(self.tiempo), list(self.acc_zg), pen='g', symbol='o')
        
        # Configurar el eje x y el eje y con etiquetas
        p3.setLabel('left', 'Temperatura (°C)')
        p3.setLabel('bottom', 'Tiempo (s)')

    def run(self):
        try:
   
            data_generator = begin_serial()
            for data in data_generator:
                print("data for",data)
                self.data = list(data)

                self.acc_x.append(self.data[0])
                self.acc_y.append(self.data[1])
                self.acc_z.append(self.data[2])

                self.gyr_x.append(self.data[3])
                self.gyr_y.append(self.data[4])
                self.gyr_z.append(self.data[5])

                self.acc_xg.append(self.data[6])
                self.acc_yg.append(self.data[7])
                self.acc_zg.append(self.data[8])

                self.tiempo.append(self.count_time)
                self.count_time += 1
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

    def plot(self):
        return self.serialThread.plot1(self.ui)

    def plot_2(self):
        return self.serialThread.plot2(self.ui)

    def plot_3(self):
        return self.serialThread.plot3(self.ui)
    
    def start_timer1(self):
        self.timer1 = QTimer()
        self.timer1.timeout.connect(self.plot)
        self.timer1.timeout.connect(self.plot_2)
        self.timer1.timeout.connect(self.plot_3)
        self.timer1.setInterval(1000)
        self.timer1.start()

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
        self.start_timer1()
        self.serialThread.start()

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
