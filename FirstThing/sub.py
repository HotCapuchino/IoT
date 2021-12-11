import paho.mqtt.client as paho
import time

TOPIC = '/home/door/state'
received = 0

def on_message(client, userdata, message):
    data = message.payload.decode("utf-8")
    print("Received message:", data)
    # print("Topic:", message.topic)
    # print("QoS:", message.qos, end='\n\n')
    global received
    received += 1

client = paho.Client("client-isu-101")
client.on_message = on_message
client.connect("broker.hivemq.com")

client.loop_start()
print("Subscribing")
client.subscribe(TOPIC, qos=1)

while True:
    if received >= 10:
        break
client.disconnect()
client.loop_stop()