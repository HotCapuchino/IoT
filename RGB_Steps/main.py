from time import sleep
import paho.mqtt.client as paho

mqtt_topic = 'andrew_LED'
broker = 'broker.emqx.io'

subscription_topic = 'lab/leds/strip/range/'
publish_topic = 'lab/leds/strip/set_leds_bytes/'

client = paho.Client('andrew-client')




class RGBStrip:
    def __init__(self, number) -> None:
        self.number = number
        self.referenced_by = set()

    def __str__(self) -> str:
        return f'RGB strip with num {self.number}, referenced by sensors: {self.referenced_by}'

    def __repr__(self) -> str:
        return self.__str__()
        

class RGBController:
    active_strips = {}
    interval_step = 36.25
    leds_num = 8


rgb_controller = RGBController()

def light_led(sensor_code, dist):
    print(f"Dist: {dist}")
    if sensor_code == 1:
        led_strip_idx = round(dist / rgb_controller.interval_step)
    else: 
        print()
        led_strip_idx = rgb_controller.leds_num - round(dist / rgb_controller.interval_step)

    print(f'led_strip_idx: {led_strip_idx} for {sensor_code} sensor')
    # nothing to light
    if led_strip_idx <= 0 or led_strip_idx > rgb_controller.leds_num:
        print(f'should be blacked out with dist: {dist}')
        for strip in rgb_controller.active_strips:
            data = [0, 0, 0] * 17
            client.publish(publish_topic + f'{strip.number}', bytearray(data))
            print()
        rgb_controller.active_strips = []
        return

    lightened = False
    for strip in rgb_controller.active_strips:
        if led_strip_idx == strip.number:
            if sensor_code not in strip.referenced_by:
                strip.referenced_by.add(sensor_code)
            lightened = True

    print(f'Active leds: {str(rgb_controller.active_strips)}')

    # light already lightened
    if lightened:   
        # print(f'RGB strip # {led_strip_idx} is already lightened!')
        return
    else: 
        for strip in rgb_controller.active_strips:
            if sensor_code in strip.referenced_by and led_strip_idx != strip.number:
                strip.referenced_by.remove(sensor_code)
                # new_references = set(filter(lambda x: x != sensor_code, strip.referenced_by))
                if len(strip.referenced_by) == 0:
                    # that means this strip has no references, thus should be blacked out
                    # print(f'RGB strip with number {strip.number} should be blacked out')
                    data = [0, 0, 0] * 17
                    client.publish(publish_topic + f'{strip.number}', bytearray(data))

        rgb_controller.active_strips = set(filter(lambda strip: len(strip.referenced_by) > 0, rgb_controller.active_strips))

        new_active = RGBStrip(led_strip_idx)
        new_active.referenced_by.add(sensor_code) 

        rgb_controller.active_strips.add(new_active)
        data = [255, 255, 255] * 17
        client.publish(publish_topic + f'{led_strip_idx}', bytearray(data))
        print("LIGHT YOU FUCKING STRIP")
        # if len(rgb_controller.active_strips) < 2:
        # else:
        #     print('It cannot be more than 2 active strips!')
        
                 

def on_message(client, userdata, message):
    data = message.payload.decode("utf-8")
    # print(float(data))
    light_led(int(str(message.topic).split('/').pop()), float(data))

client.on_message = on_message
client.connect(broker)

for i in range(2):
    client.subscribe(subscription_topic + f'{i + 1}')

client.loop_start()
while True:
    1 + 1

client.loop_stop()

client.disconnect()