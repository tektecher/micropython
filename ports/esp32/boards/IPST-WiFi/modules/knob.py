# Dev by Sonthaya Nongnuch

from machine import Pin, ADC

adc = ADC(Pin(36))
adc.atten(ADC.ATTN_11DB)
adc.width(ADC.WIDTH_12BIT)

def read():
    return adc.read()
