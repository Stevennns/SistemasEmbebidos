import serial
from struct import pack, unpack

# Set the COM port and baud rate
COM_PORT = 'COM3'  # Replace with your COM port
BAUD_RATE = 115200  # Match the baud rate used by your ESP32s2
# Open the serial connection
ser = serial.Serial(COM_PORT, BAUD_RATE, timeout = 1)


def receive_response():
    response = ser.read(32)  # Leer 72 bytes
    return response




trash = ser.read(5000)
print(trash)

message = pack('6s',"BEGIM\0".encode())
ser.write(message)

while True:
    if ser.in_waiting > 0:
        data_bytes = receive_response()
        print('data en bytes ',data_bytes)
            # Determinar el número de doubles en los datosS
        num_doubles = len(data_bytes) // 8

        #message = pack('6s',"BEGIN\0".encode())
        #ser.write(message)
            #print("bytes que usan es :" ,num_doubles)
            # Desempaquetar los datos en doubles
        try:
            data = unpack("@{}d".format(num_doubles), data_bytes)
            print(data)
        except:
            print("error")
        finally:
            continue
    

   