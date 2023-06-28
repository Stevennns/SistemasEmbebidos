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


class BMI270:
    # Definición de las variables iniciales
    def __init__(self, port):
        self.port = port

    def data(self, vector):
        self.humidity = self.bin_to_int(vector[8], vector[9], vector[10], vector[11]) / 1024.0
        self.pressure = (self.bin_to_int(vector[4], vector[5], vector[6], vector[7]) / 256.0) + 26700.0
        self.temperature = self.bin_to_int(vector[0], vector[1], vector[2], vector[3]) / 100.0

        print([self.humidity, self.pressure, self.temperature])

    # Método que permite transformar un número de 2 bytes complemento a 2 a entero con signo
    def bin_to_int(self, a, b, c, d):
        return (a << 24) | (b << 16) | (c << 8) | d

    # Método que sirve para decodificar los datos en cobs de los sensores
    def decod_COBS(self, vector):
        msn = []
        dim = len(vector)
        cont_aux = 0
        cero = vector[0]
        for l in vector:
            if l == 0 and dim > 2:
                break
            elif cont_aux == cero:
                cero = l
                msn.append(0)
                cont_aux = 0
            elif cont_aux != cero and cont_aux != 0:
                msn.append(l)
            cont_aux = cont_aux + 1
        return msn


class SerialRead:
    def __init__(self, serialPort="com3", serialBaud=115200):

        self.port = serialPort
        self.baud = serialBaud
        self.dataType = None
        self.isRun = True
        self.isReceiving = False
        self.thread = None
        self.msn = ""
        self.status = 0
        self.humidity = []
        self.temperature = []
        self.pressure = []

        self.data_h = []
        self.data_p = []
        self.data_t = []

        self.msn = "Trying to connect to: " + str(serialPort) + " at " + str(serialBaud) + " BAUD."
        try:
            self.serialConnection = serial.Serial(serialPort, serialBaud, timeout=4)
            self.BME = BMI270(self.serialConnection)
            self.msn = "Connected to " + str(serialPort) + " at " + str(serialBaud) + " BAUD."
            self.status = 1
        except:
            self.msn = "Failed to connect with " + str(serialPort) + " at " + str(serialBaud) + " BAUD."
            self.status = -1

    def readSerialStart(self):
        if self.thread == None:
            self.thread = Thread(target=self.backgroundThread)
            self.thread.start()
            while self.isReceiving != True:
                time.sleep(0.1)

    def backgroundThread(self):  # retrieve data
        time.sleep(0.1)  # give some buffer time for retrieving data
        self.serialConnection.reset_input_buffer()
        while self.isRun:
            self.serialConnection.reset_input_buffer()
            self.msn = ""
            time.sleep(0.1)  # give some buffer time for retrieving data
            while self.serialConnection.inWaiting() == 0:
                pass
            self.isReceiving = True
            self.serialConnection.readinto(self.dataType)
            self.msn = "".join(chr(x) for x in self.dataType)
            self.msn = self.msn.replace("\n", "")
            self.msn = self.msn.replace("\r", "")
            self.msn = self.msn.replace(" ", "")
            self.msn = self.msn.replace("\x00", "")
            try:
                self.msn = self.msn.encode().decode("hex")
            except:
                pass
            self.data_h = self.msn[0:8]
            self.data_p = self.msn[8:16]
            self.data_t = self.msn[16:24]
            self.msn = self.BME.decod_COBS(self.data_h)
            self.BME.data(self.msn)
            self.humidity.append(self.BME.humidity)
            self.msn = self.BME.decod_COBS(self.data_p)
            self.BME.data(self.msn)
            self.pressure.append(self.BME.pressure)
            self.msn = self.BME.decod_COBS(self.data_t)
            self.BME.data(self.msn)
            self.temperature.append(self.BME.temperature)

    def stop(self):
        self.isRun = False
        self.thread.join()
        self.serialConnection.close()

class SerialThread(QThread):
    def __init__(self):
        super().__init__()
        self.running = True

    def stop(self):
        self.running = False

    def run(self):
        #print("algo pasa")
        try:
            com = serial.Serial("com3", 115200,parity=serial.PARITY_EVEN, rtscts=1)
            print(com.read_all())
            print("Conexión serial exitosa")
            print(self.running)

            hola = "hola"
            #com.write(hola.encode())
            #com.close()
            while self.running:
                #print("read")
                t = com.read(10)
                print(t)
                print(f"texto es: {t.decode()}")
            com.close()
            print("Conexión serial Terminada")
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
        self.ui.test_boton.clicked.connect(self.leerPuerto)
        self.ui.test_boton_2.clicked.connect(self.stopSerialRead)
    def leerPuerto(self):
        print("puerto 34")
        self.serialThread.start()
        #com = serial.Serial("com3", 115200)
        #t = com.read(10)  # Leer 10 bytes
        #print(f"texto es: {t}")
        #return
        #data = self.BME.decod_COBS(aux)
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
    
    def extraer_puerto(self):
        pass
    def plot1(self):
        p = self.ui.plot_1
        # Crear los datos de tiempo y temperatura
        tiempo = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
        temperatura = [25, 24, 23, 22, 21, 20, 19, 18, 17, 16]

        # Agregar el gráfico de tiempo vs temperatura al objeto plot_1
        p.plot(tiempo, temperatura, pen='b', symbol='o')

        # Configurar el eje x y el eje y con etiquetas
        p.setLabel('left', 'Temperatura (°C)')
        p.setLabel('bottom', 'Tiempo (s)')

if __name__ == "__main__":
    import sys
    app= QtWidgets.QApplication(sys.argv)

    Dialog = QtWidgets.QDialog()
    cont = MainWindow(parent=Dialog)
    ui = cont.ui
    ui.setupUi(Dialog)
    Dialog.show()
    cont.setSignals()
    cont.plot1()
    sys.exit(app.exec_())

