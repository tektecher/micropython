# Dev by Sonthaya Nongnuch

from machine import Pin
import _thread
import utime

SW1 = Pin(8, Pin.IN, Pin.PULL_UP)
SW2 = Pin(9, Pin.IN, Pin.PULL_UP)

__sw1_press = None
__sw1_release = None
__sw2_press = None
__sw2_release = None

def __onSwitchChangesValue(pin):
    if pin.value():
        callback = None
        if pin == SW1:
            callback = __sw1_release
        elif pin == SW2:
            callback = __sw2_release
        if callback:
            # _thread.start_new_thread(callback, ())
            callback()
    else:
        callback = None
        if pin == SW1:
            callback = __sw1_press
        elif pin == SW2:
            callback = __sw2_press
        if callback:
            # _thread.start_new_thread(callback, ())
            callback()


SW1.irq(handler=__onSwitchChangesValue, trigger=Pin.IRQ_FALLING | Pin.IRQ_RISING)
SW2.irq(handler=__onSwitchChangesValue, trigger=Pin.IRQ_FALLING | Pin.IRQ_RISING)

def value(pin):
    return 0 if pin.value() else 1

def press(pin, callback):
    global __sw1_press, __sw2_press
    if pin == SW1:
        __sw1_press = callback
    elif pin == SW2:
        __sw2_press = callback

def release(pin, callback):
    global __sw1_release, __sw2_release
    if pin == SW1:
        __sw1_release = callback
    elif pin == SW2:
        __sw2_release = callback

__sw1_pressed = None
__sw2_pressed = None
__sw12_pressed = None

SW12 = 99

def SwitchLoopTask():
    sw1_press_start = None
    sw1_press_flag = False
    sw2_press_start = None
    sw2_press_flag = False
    while True:
        sw1 = SW1.value()
        sw2 = SW2.value()

        # S1
        if sw1 == 0: # S1 is press
            if sw1_press_start == None:
                sw1_press_start = utime.ticks_ms()
        else:
            if sw1_press_start != None:
                diff = utime.ticks_ms() - sw1_press_start
                sw1_press_flag = diff >= 40 and diff < 1000
                sw1_press_start = None
        
        # S2
        if sw2 == 0: # S2 is press
            if sw2_press_start == None:
                sw2_press_start = utime.ticks_ms()
        else:
            if sw2_press_start != None:
                diff = utime.ticks_ms() - sw2_press_start
                sw2_press_flag = diff >= 40 and diff < 1000
                sw2_press_start = None
        
        if sw1 == sw2 == 1:
            if sw1_press_flag and sw2_press_flag:
                if __sw12_pressed:
                    # _thread.start_new_thread(__sw12_pressed, ())
                    __sw12_pressed()
                sw1_press_flag = False
                sw2_press_flag = False
            elif sw1_press_flag:
                if __sw1_pressed:
                    # _thread.start_new_thread(__sw1_pressed, ())
                    __sw1_pressed()
                sw1_press_flag = False
            elif sw2_press_flag:
                if __sw2_pressed:
                    # _thread.start_new_thread(__sw2_pressed, ())
                    __sw2_pressed()
                sw2_press_flag = False

        utime.sleep_ms(20)
    

_thread.start_new_thread(SwitchLoopTask, ())

def pressed(pin, callback):
    global __sw1_pressed, __sw2_pressed, __sw12_pressed
    if pin == SW12:
        __sw12_pressed = callback
    elif pin == SW1:
        __sw1_pressed = callback
    elif pin == SW2:
        __sw2_pressed = callback
