# Dev by Sonthaya Nongnuch

from machine import Pin
import _thread
import utime

# create 4 pins for 4 switches 
S1 = Pin(39, Pin.IN)
S2 = Pin(23, Pin.IN)
S3 = Pin(17, Pin.IN)
S4 = Pin(16, Pin.IN)

__s1_press   = None
__s1_release = None
__s2_press   = None
__s2_release = None
__s3_press   = None
__s3_release = None
__s4_press   = None
__s4_release = None

def __onSwitchChangesValue(pin):
    if pin.value():
        callback = None
        if pin == S1:
            callback = __s1_press
        elif pin == S2:
            callback = __s2_press
        elif pin == S3:
            callback = __s3_press
        elif pin == S4:
            callback = __s4_press
        if callback:
            _thread.start_new_thread(callback, ())
    else:
        callback = None
        if pin == S1:
            callback = __s1_release
        elif pin == S2:
            callback = __s2_release
        elif pin == S3:
            callback = __s3_release
        elif pin == S4:
            callback = __s4_release
        if callback:
            _thread.start_new_thread(callback, ())

# interrupt handlers 
S1.irq(handler=__onSwitchChangesValue, trigger=Pin.IRQ_FALLING | Pin.IRQ_RISING)
S2.irq(handler=__onSwitchChangesValue, trigger=Pin.IRQ_FALLING | Pin.IRQ_RISING)
S3.irq(handler=__onSwitchChangesValue, trigger=Pin.IRQ_FALLING | Pin.IRQ_RISING)
S4.irq(handler=__onSwitchChangesValue, trigger=Pin.IRQ_FALLING | Pin.IRQ_RISING)


def value(pin):
    return 1 if pin.value() else 0

def press(pin, callback):
    global __s1_press, __s2_press, __s3_press, __s4_press
    if pin == S1:
        __s1_press = callback
    elif pin == S2:
        __s2_press = callback
    elif pin == S3:
        __s3_press = callback
    elif pin == S4:
        __s4_press = callback

def release(pin, callback):
    global __s1_release, __s2_release, __s3_release, __s4_release
    if pin == S1:
        __s1_release = callback
    elif pin == S2:
        __s2_release = callback
    elif pin == S3:
        __s3_release = callback
    elif pin == S4:
        __s4_release = callback    

__s1_pressed = None
__s2_pressed = None
__s3_pressed = None
__s4_pressed = None
__s14_pressed = None
__s23_pressed = None

S14 = 99
S23 = 98


def SwitchLoopTask():
    sw1_press_start = None
    sw1_press_flag = False
    sw2_press_start = None
    sw2_press_flag = False
    sw3_press_start = None
    sw3_press_flag = False
    sw4_press_start = None
    sw4_press_flag = False

    while True:
        sw1 = S1.value()
        sw2 = S2.value()
        sw3 = S3.value()
        sw4 = S4.value()

        # S1
        if sw1 == 1: # S1 is pressed
            if sw1_press_start == None:
                sw1_press_start = utime.ticks_ms()
        else:
            if sw1_press_start != None:
                diff = utime.ticks_ms() - sw1_press_start
                sw1_press_flag = diff >= 40 and diff < 1000
                sw1_press_start = None
        
        # S2
        if sw2 == 1: # S2 is pressed
            if sw2_press_start == None:
                sw2_press_start = utime.ticks_ms()
        else:
            if sw2_press_start != None:
                diff = utime.ticks_ms() - sw2_press_start
                sw2_press_flag = diff >= 40 and diff < 1000
                sw2_press_start = None

        # S3
        if sw3 == 1: # S3 is pressed
            if sw3_press_start == None:
                sw3_press_start = utime.ticks_ms()
        else:
            if sw3_press_start != None:
                diff = utime.ticks_ms() - sw3_press_start
                sw3_press_flag = diff >= 40 and diff < 1000
                sw3_press_start = None

        # S4
        if sw4 == 1: # S4 is pressed
            if sw4_press_start == None:
                sw4_press_start = utime.ticks_ms()
        else:
            if sw4_press_start != None:
                diff = utime.ticks_ms() - sw4_press_start
                sw4_press_flag = diff >= 40 and diff < 1000
                sw4_press_start = None
        #s1 and s4 is pressed 
        if sw1 == sw4 == 1:
            if sw1_press_flag and sw4_press_flag:
                if __s14_pressed:
                    _thread.start_new_thread(__s14_pressed, ())
                sw1_press_flag = True
                sw4_press_flag = True
            elif sw1_press_flag:
                if __s1_pressed:
                    _thread.start_new_thread(__s1_pressed, ())
                sw1_press_flag = True
            elif sw4_press_flag:
                if __s4_pressed:
                    _thread.start_new_thread(__s4_pressed, ())
                sw4_press_flag = True

        #s2 and s3 is pressed 
        if sw2 == sw3 == 1:
            if sw2_press_flag and sw3_press_flag:
                if __s23_pressed:
                    _thread.start_new_thread(__s23_pressed, ())
                sw2_press_flag = True
                sw3_press_flag = True
            elif sw2_press_flag:
                if __s2_pressed:
                    _thread.start_new_thread(__s2_pressed, ())
                sw2_press_flag = True
            elif sw3_press_flag:
                if __s3_pressed:
                    _thread.start_new_thread(__s3_pressed, ())
                sw3_press_flag = True

        utime.sleep_ms(20)
    

_thread.start_new_thread(SwitchLoopTask, ())

def pressed(pin, callback):
    global __s1_pressed, __s2_pressed, __s3_pressed, __s4_pressed, __s14_pressed, __s23_pressed
    # switch 1 and 4 
    if pin == S14:
        __s14_pressed = callback
    if pin == S1:
        __s1_pressed = callback
    elif pin == S4:
        __s4_pressed = callback
    # switch 2 and 3
    if pin == S23:
        __s23_pressed = callback
    if pin == S2:
        __s2_pressed = callback
    elif pin == S3:
        __s3_pressed = callback

