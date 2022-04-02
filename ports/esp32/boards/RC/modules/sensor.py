# Dev by Sonthaya Nongnuch
# Edited and Modified by Saeed Desouky

from machine import Pin, ADC

MAX_VALUE = const(1500)
MIN_VALUE = const(400)
TEMPERATURE_PIN = const(36)
def light():
    adc = ADC(Pin(34))
    adc.atten(ADC.ATTN_11DB)
    adc.width(ADC.WIDTH_12BIT)
    #value = adc.read()
    value = int(100 - ((adc.read() - MIN_VALUE) / (MAX_VALUE - MIN_VALUE) * 100))
    value = 100 if value > 100 else value
    value = 0 if value < 0 else value
    return value

#Developed by Saeed Desouky
def temperature():
    adc = ADC(Pin(TEMPERATURE_PIN))
    adc.atten(ADC.ATTN_11DB)
    adc.width(ADC.WIDTH_12BIT)
    temp = (float(adc.read())*4096)/1800
    temp = temp / 15
    return float(("%0.2f"  %temp))
    
