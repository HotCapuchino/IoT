import time
import paho.mqtt.client as paho
import json
import matplotlib.pyplot as plt

broker = "broker.hivemq.com"
VALUES_TOPIC = 'house/humidity'

amount_of_message_to_recieve = 10
already_recieved = 0
message_topic = None
values = {}


def on_message(client, userdata, message):
    global already_recieved
    global amount_of_message_to_recieve
    global message_topic
    time.sleep(1)
    message_topic = message.topic if not message_topic else message_topic
    data = str(message.payload.decode("utf-8"))
    already_recieved += 1
    recieved_obj = json.loads(data)
    values.update({str(recieved_obj['value']): recieved_obj['time']})


def buildPlot():
    plt.plot(list(values.keys()), list(values.values()), '-o')
    plt.show()


client = paho.Client("andrew_subsriber")
client.on_message = on_message

print("Connecting to broker", broker)
client.connect(broker)
client.loop_start()
print("Subscribing")

client.subscribe("house/humidity")

while True:
    if already_recieved == amount_of_message_to_recieve:
        client.disconnect()
        client.loop_stop()
        print(f"message topic is {message_topic}")
        buildPlot()
        break
