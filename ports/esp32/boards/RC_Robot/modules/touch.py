#developed by Saeed Desouky
from time import sleep
from machine import Pin, PWM, TouchPad
import _thread

# Initialize touch pins 
touch = {
    1: TouchPad(Pin(14)),
    3: TouchPad(Pin(2)),
    4: TouchPad(Pin(27)),
    6: TouchPad(Pin(33)),  
    7: TouchPad(Pin(32)),
    9: TouchPad(Pin(4))}
#store values for caliperation
threshold = {
    1: [],
    3: [],
    4: [],
    6: [],
    7: [],
    9: []}

def touched (pin, callback):
  global touch, threshold
  # Scan each TouchPad 12 times
  for x in range(12):
     threshold[pin].append(touch[pin].read())
     sleep(.1)
  # Store average threshold values
  threshold[pin] = sum(threshold[pin]) // len(threshold[pin])  
  while True:
    capacitance = touch[pin].read()
    cap_ratio = capacitance / threshold[pin]
    if .20 < cap_ratio < .90:
      if callback: 
       _thread.start_new_thread(callback, ())
       sleep(.2)  # Debounce button press
    sleep(.1)

def value (pin):  
  capacitance = touch[pin].read()
  return capacitance  

