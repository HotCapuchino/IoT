import serial
import paho.mqtt.client as paho
import json

MESSAGE_LENGTH = {b'1': 5, b'2': 5}


def get_connection():
    ser = serial.Serial('COM7', timeout=1)
    return ser


def get_sensor(ser, sensor_byte):
    ser.write(sensor_byte)
    data = ser.read(MESSAGE_LENGTH[sensor_byte]).decode().strip()
    if data == '':
        return None
    return data


ser = get_connection()

client = paho.Client('andrew_publisher')
client.connect("broker.hivemq.com")

if ser:
    client.loop_start()

for i in range(10):
    sensor_data = get_sensor(ser, b'1')
    # print(f'data is (try {i})', get_sensor(ser, b'1'))
    if sensor_data:
        client.publish('arduino/data', sensor_data, qos=1)

ser.close()
client.loop_stop()
