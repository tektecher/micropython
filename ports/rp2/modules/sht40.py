# Dev by Sonthaya Nongnuch

from machine import Pin, I2C
from utime import sleep

ADDR = 0x44

i2c = I2C(0, scl=Pin(1), sda=Pin(0), freq=100000) 

def read():
    for _ in range(5):
        try:
            i2c.writeto(ADDR, b'\xFD')
            sleep(0.01)
            rx_bytes = i2c.readfrom(ADDR, 6)
        except:
            continue

        t_ticks = rx_bytes[0] * 256 + rx_bytes[1]
        rh_ticks = rx_bytes[3] * 256 + rx_bytes[4]

        t_degC = round(-45 + 175 * t_ticks / 65535, 2)
        rh_pRH = round(-6 + 125 * rh_ticks / 65535, 2)
        rh_pRH = max(0, min(100, rh_pRH), rh_pRH)

        return (t_degC, rh_pRH)
    
    return (-999, -999)
