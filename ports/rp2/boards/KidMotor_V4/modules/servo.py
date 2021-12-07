# Dev by Sonthaya Nongnuch

from machine import Pin, PWM

SV1 = Pin(18)
SV2 = Pin(19)
SV3 = Pin(20)

SV1_obj = PWM(SV1)
SV1_obj.freq(50)
SV1_obj.duty_ns(0)

SV2_obj = PWM(SV2)
SV2_obj.freq(50)
SV2_obj.duty_ns(0)

SV3_obj = PWM(SV3)
SV3_obj.freq(50)
SV3_obj.duty_ns(0)

MIN = 500000
MAX = 2500000

def angle(pin, angle):
    angle_to_ns = int(MIN + ((angle / 180.0) * (MAX - MIN)))
    if pin == SV1:
        SV1_obj.duty_ns(angle_to_ns)
    elif pin == SV2:
        SV2_obj.duty_ns(angle_to_ns)
    elif pin == SV3:
        SV3_obj.duty_ns(angle_to_ns)
    else:
        servo_tmp = PWM(pin, freq=50)
        servo_tmp.freq(50)
        servo_tmp.duty_ns(angle_to_ns)

def timing(min_ns=500000, max_ns=2500000):
    global MIN, MAX
    MIN = min_ns
    MAX = max_ns
