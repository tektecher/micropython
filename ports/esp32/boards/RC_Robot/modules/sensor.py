# Dev by Sonthaya Nongnuch
#poerted by Saeed Desouky
from machine import Pin, ADC, time_pulse_us
from time import sleep_us

MAX_VALUE = const(1500)
MIN_VALUE = const(400)
TEMPERATURE_PIN = const(36)

#line follower sensors 
def light1_raw():
    return analogRead(39)

def light2_raw():
    return analogRead(35)

def light3_raw():
    return analogRead(33)

__white_threshold = 3000
__black_threshold = 2000
def white_threshold(threshold=None):
    if threshold != None:
        __white_threshold = threshold
    return __white_threshold

def black_threshold(threshold=None):
    global __white_threshold
    if threshold != None:
        __black_threshold = threshold
    return __black_threshold

def light1_is_black():
    return True if light1_raw() <= __black_threshold else False

def light2_is_black():
    return True if light2_raw() <= __black_threshold else False

def light3_is_black():
    return True if light3_raw() <= __black_threshold else False

def light1_is_white():
    return True if light1_raw() >= __white_threshold else False

def light2_is_white():
    return True if light2_raw() >= __white_threshold else False

def light3_is_white():
    return True if light3_raw() >= __white_threshold else False

#light, temperature and ultrasonic sensors 
#Developed by Saeed Desouky
def light():
    adc = ADC(Pin(34))
    adc.atten(ADC.ATTN_11DB)
    adc.width(ADC.WIDTH_12BIT)
    #value = adc.read()
    value = int(100 - ((adc.read() - MIN_VALUE) / (MAX_VALUE - MIN_VALUE) * 100))
    value = 100 if value > 100 else value
    value = 0 if value < 0 else value
    return value


def temperature():
    adc = ADC(Pin(TEMPERATURE_PIN))
    adc.atten(ADC.ATTN_11DB)
    adc.width(ADC.WIDTH_12BIT)
    temp = (float(adc.read())*4096)/1800
    temp = temp / 15
    return float(("%0.2f"  %temp))


def analogRead(pin_n):
    adc = ADC(Pin(pin_n))
    adc.atten(ADC.ATTN_11DB)
    adc.width(ADC.WIDTH_12BIT)
    return adc.read()


def read(trig_pin, echo_pin):
    trigger = Pin(trig_pin, mode=Pin.OUT)
    echo = Pin(echo_pin, mode=Pin.IN)

    trigger.value(0)
    sleep_us(5)
    trigger.value(1)
    sleep_us(10)
    trigger.value(0)

    try:
        pulse_time = time_pulse_us(echo, 1, 1000000)
        d = (pulse_time / 2) / 29.1
        return int(d) if d < 400 else -1
    except OSError as ex:
        return -1


