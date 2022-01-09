# Dev by Sonthaya Nongnuch

from machine import Pin, I2C, ADC

TMP75_ADDR = const(0x48)

i2c1 = I2C(1, scl=Pin(21), sda=Pin(22), freq=100000)
i2c1.writeto_mem(TMP75_ADDR, 0x01, '\x00')

def temperature():
    data = i2c1.readfrom_mem(TMP75_ADDR, 0x00, 2)
    temp = ((data[0] << 8) | data[1]) >> 4
    temp = temp * 0.0625
    temp = temp * (-1 if (data[0] & 0x80) != 0 else 1)
    return temp

def microphone():
    adc = ADC(Pin(39))
    adc.atten(ADC.ATTN_11DB)
    adc.width(ADC.WIDTH_12BIT)
    return adc.read()
