import serial
import sys
import time
import random
import numpy as np


def get_connection(port):
    try:
        return serial.Serial(port)
    except Exception as e:
        print('Oops! Something went wrong!')


def find_led(leds, mode='blind'):
    led_index = None
    for i in range(len(leds)):
        led = leds[i]
        color_sum = 0
        for color in led:
            color_sum += color
        if mode == 'blind':
            if color_sum == 0:
                led_index = i
                break
        else:
            if color_sum > 0:
                led_index = i
                break
    return led_index


def is_fully_lightened(leds):
    color_sum = 0
    for color in leds[len(leds) - 1]:
        color_sum += color
    if color_sum > 0:
        return True
    else:
        return False


def turn_off_picture(ser, leds):
    first_lightened_led_index = find_led(leds, 'light')
    if first_lightened_led_index >= 0:
        leds[first_lightened_led_index] = bytearray([0, 0, 0])
    leds[0] = bytearray([0, 0, 0])
    ser.write(bytearray(list(np.array(leds).flatten())))


def draw_picture(ser, leds):
    first_blind_led_index = find_led(leds)
    if first_blind_led_index >= 0:
        r = random.randint(0, 255)
        g = random.randint(0, 255)
        b = random.randint(0, 255)
        leds[first_blind_led_index] = [r, g, b]
    ser.write(bytearray(list(np.array(leds).flatten())))


if __name__ == "__main__":
    port = input('Enter port of device: ')

    ser = None
    if port:
        ser = get_connection(port)
    if not ser:
        sys.exit(-1)

    counter = 1
    lightened = False

    leds_num = 0
    while not leds_num:
        leds_num = int(input('Enter number of leds you want to use, (number should be greater then 3): '))
        if leds_num < 3:
            leds_num = 0

    pause = 0
    while not pause:
        pause = float(input('Enter single led pause in seconds, (max amount of seconds - 10, min - 1): '))
        if pause > 10 or pause < 1:
            pause = 0

    leds = []
    for i in range(leds_num):
        leds.append(bytearray([0, 0, 0]))
    while (True):
        has_light = is_fully_lightened(leds)
        if has_light != lightened:
            lightened = has_light
            if counter < 9:
                counter += 1
        if has_light:
            turn_off_picture(ser, leds)
        else:
            draw_picture(ser, leds)
        time.sleep(pause / counter)
