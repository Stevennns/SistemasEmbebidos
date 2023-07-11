from struct import pack
import serial
# Set the COM port and baud rate
COM_PORT = 'COM3'  # Replace with your COM port
BAUD_RATE = 115200  # Match the baud rate used by your ESP32s2

# Open the serial connection
ser = serial.Serial(COM_PORT, BAUD_RATE, timeout = 1)

array = [6,3,10,2]
packed_array = pack('4i', *array)
ser.write(packed_array)