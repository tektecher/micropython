# Dev by Sonthaya Nongnuch

from machine import Pin
from neopixel import NeoPixel
import display
import buzzer

np = NeoPixel(Pin(12, Pin.OUT), 3)
np.fill((0, 0, 0))
np.write()

buzzer.off()
