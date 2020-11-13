from machine import Pin
import display


Pin(17, Pin.OUT).value(1)
Pin(2, Pin.OUT).value(1)
Pin(15, Pin.OUT).value(1)
Pin(12, Pin.OUT).value(1)
display.clear()
