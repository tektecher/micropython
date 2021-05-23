"""
from machine import Pin
import display
import buzzer

Pin(25, Pin.OUT).value(0) # LED2
Pin(23, Pin.OUT).value(0) # LDE1
display.fill(0)
display.show()
buzzer.off()
"""
