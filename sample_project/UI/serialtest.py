import serial

ser = serial.Serial('COM3', baudrate=115200)

def send_command(command):
    ser.write(command.encode('utf-8'))
    print("Comando enviado:", command)

    #response = ser.readline().decode('utf-8').rstrip()
    #print("Respuesta recibida:", response)
    #return 
def leer():
    response = ser.readline().decode('utf-8').rstrip()
    print("Respuesta recibida:", response)
    leer()
send_command("PING")  # Enviar el comando "PING" a la ESP32

ser.close()
