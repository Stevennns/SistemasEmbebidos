import serial

ser = serial.Serial('com3', baudrate=115200)


def llamar(i):
    if i == 0:
        while True:
            if ser.in_waiting > 0:
                data = ser.read(ser.in_waiting).decode('utf-8').rstrip()
    else:

        data = "Hola desde Python"  # Datos a ser escritos

        ser.write(data.encode('utf-8'))  # Escribe los datos en el puerto serial

        print("Datos escritos con éxito:", data)

        ser.close()  # Cierra la conexión serial

llamar(0)