import time
import paho.mqtt.client as paho
import random
import json

broker="broker.hivemq.com"
REQUIRED_QoS = 1

client = paho.Client("andrew_publisher")

messages_amount = 10

print("Connecting to broker", broker)
client.connect(broker)
client.loop_start()
print("Publishing")

for i in range(messages_amount):
    state = json.dumps({
        'time': time.time(),
        'value': random.randint(0, 100)
    })
    print(f"state is {state}")
    client.publish("house/humidity", state, qos=REQUIRED_QoS)
    time.sleep(random.randint(0, 1))

client.disconnect()
client.loop_stop()