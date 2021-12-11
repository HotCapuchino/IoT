import serial
import paho.mqtt.client as paho
import sys

PORT = '/dev/ttyUSB0'
TOPIC = '/home/door/state'

def get_connection():
    ser = serial.Serial(PORT)
    return ser

def get_sensor(ser, sensor_byte):
    data = ser.read(sensor_byte)
    if data == b'\x01':
        return  'Door opened'
    else: 
        return 'Door closed' 

ser = get_connection()
client = paho.Client('andrew_publisher')
client.connect("broker.hivemq.com")

if not ser:
    sys.exit(-1)

client.loop_start()
for i in range(100):
    door_state = get_sensor(ser, 1)
    if door_state:
        client.publish(TOPIC, door_state, qos=1)
        print(door_state)  

ser.close()
client.loop_stop()  