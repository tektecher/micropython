# Example using PIO to drive a set of WS2812 LEDs.
# Dev by Sonthaya Nongnuch

import array
from machine import Pin
import rp2
from utime import sleep_ms

@rp2.asm_pio(sideset_init=rp2.PIO.OUT_LOW, out_shiftdir=rp2.PIO.SHIFT_LEFT, autopull=True, pull_thresh=24)
def ws2812():
    T1 = 2
    T2 = 5
    T3 = 3
    wrap_target()
    label("bitloop")
    out(x, 1)               .side(0)    [T3 - 1]
    jmp(not_x, "do_zero")   .side(1)    [T1 - 1]
    jmp("bitloop")          .side(1)    [T2 - 1]
    label("do_zero")
    nop()                   .side(0)    [T2 - 1]
    wrap()


# Create the StateMachine with the ws2812 program, outputting on Pin(23).
sm = rp2.StateMachine(0, ws2812, freq=8_000_000, sideset_base=Pin(23))

# Start the StateMachine, it will wait for data on its FIFO.
sm.active(1)

# Display a pattern on the LEDs via an array of LED RGB values.
ar = array.array("I", [ 0 ])

brightness = 50

def colorRGB(r, g, b):
    global brightness
    r = r * brightness / 100.0
    g = g * brightness / 100.0
    b = b * brightness / 100.0
    ar[0] = (int(g) << 16) | (int(r) << 8) | int(b)
    sm.put(ar, 8)

def color(c):
    r = (c >> 16) & 0xFF
    g = (c >> 8) & 0xFF
    b = (c >> 0) & 0xFF
    colorRGB(r, g, b)

def bright(value=None):
    global brightness
    if value is None:
        return brightness
    else:
        brightness = min(max(0, int(value)), 100)

def rainbow(wait):
    for j in range(256):
        WheelPos = j & 255
        if WheelPos < 85:
            colorRGB(WheelPos * 3, 255 - WheelPos * 3, 0)
        elif WheelPos < 170:
            WheelPos -= 85
            colorRGB(255 - WheelPos * 3, 0, WheelPos * 3)
        else:
            WheelPos -= 170
            colorRGB(0, WheelPos * 3, 255 - WheelPos * 3)
        sleep_ms(wait)