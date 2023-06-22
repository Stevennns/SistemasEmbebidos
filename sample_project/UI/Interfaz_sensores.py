import sys,os
from PyQt5 import QtCore, QtGui, uic, QtWidgets
from threading import Thread
from interfaz import Ui_Dialog
import serial
import pyqtgraph
import numpy
import os.path as path
import time
import scipy.io

import pygame

class BME280:
    #Definición de las variables iniciales
    def __init__(self, port):
        self.port = port     

    def data(self,vector):
        self.humidity = self.bin_to_int(vector[8],vector[9],vector[10],vector[11])/1024.0
        self.pressure = (self.bin_to_int(vector[4],vector[5],vector[6],vector[7])/256.0)+26700.0
        self.temperature = self.bin_to_int(vector[0],vector[1],vector[2],vector[3])/100.0

        #print([self.humidity, self.pressure, self.temperature])
    #metodo que permite transformar un numero de 2 bytes complemento a 2 a entero con signo 
    def bin_to_int(self,a,b,c,d):

        return (a<<24)|(b<<16)|(c<<8)|d

        
    ##metodo que sirve para decodificar los datos en cobs de los sensores        
    def decod_COBS(self,vector):
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
    def __init__(self, serialPort='com4', serialBaud=115200):
        self.port = serialPort
        self.baud = serialBaud
        self.dataType = None
        self.isRun = True
        self.isReceiving = False
        self.thread = None
        self.msn =''
        self.status = 0
        self.humidity = []
        self.temperature = []
        self.pressure = []

        self.data_h = []
        self.data_p = []
        self.data_t = []

        self.msn ='Trying to connect to: ' + str(serialPort) + ' at ' + str(serialBaud) + ' BAUD.'
        try:
            self.serialConnection = serial.Serial(serialPort, serialBaud, timeout=4)
            self.BME = BME280(self.serialConnection)
            self.msn ='Connected to ' + str(serialPort) + ' at ' + str(serialBaud) + ' BAUD.'
            self.status = 1
        except:
            self.msn ="Failed to connect with " + str(serialPort) + ' at ' + str(serialBaud) + ' BAUD.'
            self.status = 0
            pass

    def readSerialStart(self):
        if self.thread == None:
            self.humidity = []
            self.temperature = []
            self.pressure = []
            self.isRun = True
            self.thread = Thread(target=self.backgroundThread)
            self.thread.start()

            # Block till we start receiving values
            while self.isReceiving != True:
                time.sleep(0.1)

    def backgroundThread(self):    # retrieve data
        time.sleep(1.0)  # give some buffer time for retrieving data
        self.serialConnection.reset_input_buffer()
        while (self.isRun):
            aux = self.serialConnection.read_until(b'\x00')
            data = self.BME.decod_COBS(aux)
            if(len(data)==12):
                self.BME.data(data)
                self.humidity.append(self.BME.humidity)
                self.temperature.append(self.BME.temperature)
                self.pressure.append(self.BME.pressure)

                self.data_h = numpy.append(self.data_h[1:],self.BME.humidity)
                self.data_t = numpy.append(self.data_t[1:],self.BME.temperature)
                self.data_p = numpy.append(self.data_p[1:],self.BME.pressure/101325.0)

            self.isReceiving = True

    def stop(self):
        self.isRun = False
        self.thread.join()
        self.thread = None

    def close(self):
        self.serialConnection.close()
        self.msn =str(self.port) + ' disconnected.'
        self.status = 0



class MainApp(QtWidgets.QMainWindow):
    def __init__(self):
        QtWidgets.QMainWindow.__init__(self)
        self.MainWindow = QtWidgets.QMainWindow()
        self.MainWindow.ui = uic.loadUi('respiratory_interface.ui')
        self.MainWindow.ui.setWindowTitle('Respiratory Interface')

        self.display_seconds = 10
        self.sampling_period_ms = 100 
        self.plot_init()

        self.plots_update_timer = QtCore.QTimer()

        pygame.mixer.init()
        
        self.MainWindow.ui.conn_button.clicked.connect(self.connect)
        self.MainWindow.ui.init_button.clicked.connect(self.init_ADQ)
        self.MainWindow.ui.save_button.clicked.connect(self.saveSignals)
        self.plots_update_timer.timeout.connect(self.plots_update)

        self.MainWindow.ui.show()


    def connect(self):
        if self.MainWindow.ui.conn_button.text() == 'CONECTAR':
            try:
                if self.MainWindow.ui.port.text() != '':
                    self.s = SerialRead(serialPort =self.MainWindow.ui.port.text(), serialBaud = 115200)
                    print(self.s.msn)
                    if self.s.status == 1:
                        
                        self.MainWindow.ui.conn_button.setText('DESCONECTAR') 
            except:
                print("Error Conexión")
                pass
        
        elif self.MainWindow.ui.conn_button.text() == 'DESCONECTAR' and self.s.status == 1:
            try:
                self.s.close()
                print(self.s.msn)
                if self.s.status == 0:
                    self.MainWindow.ui.conn_button.setText('CONECTAR')
            except:
                print("Error Desconexión")
                pass
 

    def init_ADQ(self):        
        self.pres = []
        self.tmp = []

        try:
            if self.MainWindow.ui.init_button.text() == 'INICIAR' and self.s.status == 1:
                self.s.data_t = numpy.zeros(self.data_length)
                self.s.data_p = numpy.zeros(self.data_length)
                self.s.data_h = numpy.zeros(self.data_length)
                self.s.readSerialStart()  # starts background thread
                self.plots_update_timer.start(10)
                self.MainWindow.ui.init_button.setText('DETENER')
                pygame.mixer.music.load("sonidos.mpeg")
                pygame.mixer.music.play()
            
            elif self.MainWindow.ui.init_button.text() == 'DETENER' and self.s.status == 1:
                self.s.stop()
                self.plots_update_timer.stop()
                self.MainWindow.ui.init_button.setText('INICIAR')
        except:
            pass
    
    def plot_init(self):
        # Graphics Plots data
        self.data_length = int((self.display_seconds*1000)/self.sampling_period_ms)
        self.time = numpy.linspace(0,self.display_seconds,self.data_length)
        self.data1 = numpy.zeros(self.data_length)
        self.data2 = numpy.zeros(self.data_length)
        self.data3 = numpy.zeros(self.data_length)

        # Graficas
        self.w1 = self.MainWindow.ui.view.addPlot()
        self.w1.setLabel('left', 'Temperatura', units ='degC')
        self.w1.setLabel('bottom', 'Tiempo', units ='Seg')
        self.w1.setYRange(15, 30, padding=0)
        self.w1.setXRange(0, self.display_seconds, padding=0)
        self.w1_axis = self.w1.getAxis('left')
        self.w1_axis.setTickSpacing(15, 5)
        self.w1_axis = self.w1.getAxis('bottom')
        self.w1_axis.setTickSpacing(self.display_seconds,1)
        self.w1.setMenuEnabled(True)
        self.w1.setMouseEnabled(x = False, y = True)
        self.w1.hideButtons()
        self.MainWindow.ui.view.nextRow()

        self.w2 = self.MainWindow.ui.view.addPlot()
        self.w2.setLabel('left', 'Presión', units ='atm')
        self.w2.setLabel('bottom', 'Tiempo', units ='Seg')
        self.w2.setYRange(0.9, 1.2, padding=0)
        self.w2.setXRange(0, self.display_seconds, padding=0)
        self.w2_axis = self.w2.getAxis('left')
        self.w2_axis.setTickSpacing(40, 10)
        self.w2_axis = self.w2.getAxis('bottom')
        self.w2_axis.setTickSpacing(self.display_seconds,1)
        self.w2.setMenuEnabled(True)
        self.w2.setMouseEnabled(x = False, y = True)
        self.w2.hideButtons()
        self.MainWindow.ui.view.nextRow()

        self.w3 = self.MainWindow.ui.view.addPlot()
        self.w3.setLabel('left', 'Humedad', units ='hr')
        self.w3.setLabel('bottom', 'Tiempo', units ='Seg')
        self.w3.setYRange(40, 80, padding=0)
        self.w3.setXRange(0, self.display_seconds, padding=0)
        self.w3_axis = self.w3.getAxis('left')
        self.w3_axis.setTickSpacing(200, 50)
        self.w3_axis = self.w3.getAxis('bottom')
        self.w3.setMenuEnabled(True)
        self.w3.setMouseEnabled(x = False, y = True)
        self.w3.hideButtons()
        self.w3_axis.setTickSpacing(self.display_seconds,1)
  

        # Init Curves
        self.curve1_1 = self.w1.plot(self.data1, pen=pyqtgraph.mkPen(color = (255,255,0), width = 2, style=QtCore.Qt.SolidLine))
        self.curve2_1 = self.w2.plot(self.data2, pen=pyqtgraph.mkPen(color = (0,255,0), width = 2, style=QtCore.Qt.SolidLine))
        self.curve3_1 = self.w3.plot(self.data3, pen=pyqtgraph.mkPen(color = (0,255,255), width = 2, style=QtCore.Qt.SolidLine))
    
    def plots_update(self):
        # Plot update
        self.curve1_1.setData(self.time,self.s.data_t)
        self.curve2_1.setData(self.time,self.s.data_p)
        self.curve3_1.setData(self.time,self.s.data_h)

    def saveSignals(self):
        test_name = self.MainWindow.ui.test_name.text()
        mydata = numpy.array([self.s.pressure,self.s.temperature,self.s.humidity])
        scipy.io.savemat(test_name+'.mat',{'mydata':mydata})
        numpy.save(test_name, mydata)


if __name__ == "__main__":
   app = QtWidgets.QApplication(sys.argv)
   win = MainApp()
   sys.exit(app.exec_())