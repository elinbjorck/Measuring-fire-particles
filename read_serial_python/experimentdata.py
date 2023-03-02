import serial

ser = serial.Serial('COM6', 115200, timeout = 1)
incoming = ''
while True:
  if ser.is_open:
    incoming = ser.readline()
  else:
    incoming = 'port is not open'
  print(incoming)