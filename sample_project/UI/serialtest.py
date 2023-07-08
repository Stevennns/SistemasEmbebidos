import serial
from struct import pack, unpack

# Set the COM port and baud rate
COM_PORT = 'COM3'  # Replace with your COM port
BAUD_RATE = 115200  # Match the baud rate used by your ESP32s2

# Open the serial connection
ser = serial.Serial(COM_PORT, BAUD_RATE, timeout = 1)

# Functions 
def send_message(message):
    ser.write(message)

#def receive_response():
    #response = ser.read_until(b'\x00')  # Reading until \0
    #return response[:-1]
def receive_response():
    response = ser.read(72)  # Leer 72 bytes
    return response

def receive_data():
    data_bytes = receive_response()
    #print(data_bytes)
    # Determinar el número de doubles en los datosS
    num_doubles = len(data_bytes) // 8
    #print("bytes que usan es :" ,num_doubles)
    # Desempaquetar los datos en doubles
    data = unpack("@{}d".format(num_doubles), data_bytes)
    #data = unpack("<{}d", data)
    #print(data)
    print(f'Received: {data}')
    return data

def send_end_message():
    end_message = pack('4s', 'END\0'.encode())
    ser.write(end_message)

def begin_serial():
    # Send "BEGIN" message
    message = pack('6s','BEGIN\0'.encode())
    ser.write(message)

    # Read data from the serial port, waiting for the data
    counter = 0
    while True:
        if ser.in_waiting > 0:
            try:
                message = receive_data()
            
            except Exception as inst:

                print('Error en leer mensaje')
                print(inst)
                continue
            else: 
                counter += 1
                print(counter)
            finally:
                if counter == 10:
                    print('Lecturas listas!')
                    break

    # Sending message to end data sending
    send_end_message()

    # Waiting for message OK to end communications
    while True:
        if ser.in_waiting > 0:
            try:
                message = receive_response()
                msg = message.decode('utf-8')
            except:
                print('Error en leer mensaje')
                continue
            else: 
                if msg == 'OK':
                    print('Cerrando conexión...')
                    break
    ser.close()
        