# Dev by Sonthaya Nongnuch

from machine import Pin, PWM
from time import sleep
from micropython import const

FORWARD     = const(1)
BACKWARD    = const(2)
TURN_LEFT   = const(3)
TURN_RIGHT  = const(4)

__m1_a = PWM(Pin(13))
__m1_a.freq(1000)

__m1_b = PWM(Pin(14))
__m1_b.freq(1000)

__m2_a = PWM(Pin(17))
__m2_a.freq(1000)

__m2_b = PWM(Pin(16))
__m2_b.freq(1000)

def wheel(speed_left, speed_right):
    dir1 = 0 if speed_left >= 0 else 1
    if speed_left < 0:
        speed_left = speed_left * -1
    speed_left = 65535 - min(max(int(speed_left / 100 * 65535), 0), 65535)
    __m1_a.duty_u16(65535 if dir1 == 1 else speed_left)
    __m1_b.duty_u16(speed_left if dir1 == 1 else 65535)

    dir2 = 1 if speed_right >= 0 else 0
    if speed_right < 0:
        speed_right = speed_right * -1
    speed_right = 65535 - min(max(int(speed_right / 100 * 65535), 0), 65535)
    __m2_a.duty_u16(65535 if dir2 == 1 else speed_right)
    __m2_b.duty_u16(speed_right if dir2 == 1 else 65535)

def forward(speed=50, time=1):
    wheel(speed, speed)
    sleep(time)
    wheel(0, 0)

def backward(speed=50, time=1):
    wheel(speed * -1, speed * -1)
    sleep(time)
    wheel(0, 0)

def turn_left(speed=50, time=1):
    wheel(0, speed)
    sleep(time)
    wheel(0, 0)

def turn_right(speed=50, time=1):
    wheel(speed, 0)
    sleep(time)
    wheel(0, 0)

def move(dir, speed):
    if dir == FORWARD:
        wheel(speed, speed)
    elif dir == BACKWARD:
        wheel(speed * -1, speed * -1)
    elif dir == TURN_LEFT:
        wheel(0, speed)
    elif dir == TURN_RIGHT:
        wheel(speed, 0)

def stop():
    wheel(0, 0)
