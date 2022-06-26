from micropython import const
import framebuf
from machine import Pin, I2C
import utime
import math

# register definitions
SET_CONTRAST = const(0x81)
SET_ENTIRE_ON = const(0xA4)
SET_NORM_INV = const(0xA6)
SET_DISP = const(0xAE)
SET_MEM_ADDR = const(0x20)
SET_COL_ADDR = const(0x21)
SET_PAGE_ADDR = const(0x22)
SET_DISP_START_LINE = const(0x40)
SET_SEG_REMAP = const(0xA0)
SET_MUX_RATIO = const(0xA8)
SET_COM_OUT_DIR = const(0xC0)
SET_DISP_OFFSET = const(0xD3)
SET_COM_PIN_CFG = const(0xDA)
SET_DISP_CLK_DIV = const(0xD5)
SET_PRECHARGE = const(0xD9)
SET_VCOM_DESEL = const(0xDB)
SET_CHARGE_PUMP = const(0x8D)

ADDR = const(0x3C)
pages = 64 // 8

i2c = I2C(0, scl=Pin(22), sda=Pin(21), freq=100000)
buffer = bytearray(pages * 128)
fbuff = framebuf.FrameBuffer(buffer, 128, 64, framebuf.MONO_VLSB)

def write_cmd(cmd):
    i2c.writeto(ADDR, bytes([ 0x80, cmd ]))

def write_data(buf):
    write_list = [b"\x40", None]  # Co=0, D/C#=1
    write_list[1] = buf
    i2c.writevto(ADDR, write_list)

def show():
    write_cmd(SET_COL_ADDR)
    write_cmd(0)
    write_cmd(128 - 1)
    write_cmd(SET_PAGE_ADDR)
    write_cmd(0)
    write_cmd(pages - 1)
    write_data(buffer)

def poweroff():
    write_cmd(SET_DISP | 0x00)

def poweron():
    write_cmd(SET_DISP | 0x01)

def contrast(contrast):
    write_cmd(SET_CONTRAST)
    write_cmd(contrast)

def invert(invert):
    write_cmd(SET_NORM_INV | (invert & 1))

def fill(c):
    fbuff.fill(c)

def pixel(x, y, c):
    fbuff.pixel(x, y, c)

def scroll(dx, dy):
    fbuff.scroll(dx, dy)

def text(string, x, y, c=1):
    fbuff.text(string, x, y, c)

def hline(x, y, w, c):
    fbuff.hline(x, y, w, c)

def vline(x, y, h, c):
    fbuff.vline(x, y, h, c)

def line(x1, y1, x2, y2, c):
    fbuff.line(x1, y1, x2, y2, c)

def rect(x, y, w, h, c):
    fbuff.rect(x, y, w, h, c)

def fill_rect(x, y, w, h, c):
    fbuff.fill_rect(x, y, w, h, c)

def blit(fbuf, x, y):
    fbuff.blit(fbuf, x, y)

def image(imageData, x, y):
    buffer = bytearray(imageData[2:])
    fbuff.blit(framebuf.FrameBuffer(buffer, int(imageData[0]), int(imageData[1]), framebuf.MONO_HLSB), x, y)
    buffer = None

#================= this section under Development========================
#created and ported by Saeed Desouky 
#=====================start of circle==================
#empty circle 
def circle(x,y,r,c): 
    fbuff.pixel(x-r,y,c)
    fbuff.pixel(x+r,y,c)
    fbuff.pixel(x,y-r,c)
    fbuff.pixel(x,y+r,c)
    for i in range(1,r):
        a = int(math.sqrt(r*r-i*i))
        fbuff.pixel(x-a,y-i,c)
        fbuff.pixel(x+a,y-i,c)
        fbuff.pixel(x-a,y+i,c)
        fbuff.pixel(x+a,y+i,c)
        fbuff.pixel(x-i,y-a,c)
        fbuff.pixel(x+i,y-a,c)
        fbuff.pixel(x-i,y+a,c)
        fbuff.pixel(x+i,y+a,c)
#filled circle 
def fill_circle(x,y,r,c): 
    fbuff.hline(x-r,y,r*2,c)
    for i in range(1,r):
        a = int(math.sqrt(r*r-i*i)) # Pythagoras!
        fbuff.hline(x-a,y+i,a*2,c) # Lower half
        fbuff.hline(x-a,y-i,a*2,c) # Upper half

#=====================End of circle==================

#============== Draw sin() and cos() =================
#================================================================
#sin()
def Draw_Sin():
    factor = 342 /120
    fbuff.hline(0,32,128,1)    
    for x in range(0,128):
        y = int ((math.sin(math.radians(x * factor)))* -30) + 32
        fbuff.pixel(x,y,1)
        show()
#cos()
def Draw_Cos():
    factor =  342 /120
    fbuff.hline(0,32,128,1)    
    for x in range(0,128):
        y = int((math.cos(math.radians(x * factor)))* -30) + 32
        fbuff.pixel(x,y,1)
        show()
#============= End of graph ==========================

 #=====================start of triangle==================
# Modified from https://github.com/SpiderMaf/PiPicoDsply/blob/main/filled-triangles.py
# To work on RC display displays
class Point:
    def __init__(self,x,y):
        self.X=x
        self.Y=y
    def __str__(self):
        return "Point(%s,%s)"%(self.X,self.Y)
class Triangle:
    def __init__(self,p1,p2,p3):
        self.P1=p1
        self.P2=p2
        self.P3=p3

    def __str__(self):
        return "Triangle(%s,%s,%s)"%(self.P1,self.P2,self.P3)
    
    def draw(self):
        print("I should draw now")
        self.fillTri()
    # Filled triangle routines ported from http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html      
    def sortVerticesAscendingByY(self):    
        if self.P1.Y > self.P2.Y:
            vTmp = self.P1
            self.P1 = self.P2
            self.P2 = vTmp
        
        if self.P1.Y > self.P3.Y:
            vTmp = self.P1
            self.P1 = self.P3
            self.P3 = vTmp

        if self.P2.Y > self.P3.Y:
            vTmp = self.P2
            self.P2 = self.P3
            self.P3 = vTmp
        
    def fillTri(self):
        self.sortVerticesAscendingByY()
        if self.P2.Y == self.P3.Y:
            fillBottomFlatTriangle(self.P1, self.P2, self.P3)
        else:
            if self.P1.Y == self.P2.Y:
                fillTopFlatTriangle(self.P1, self.P2, self.P3)
            else:
                newx = int(self.P1.X + (float(self.P2.Y - self.P1.Y) / float(self.P3.Y - self.P1.Y)) * (self.P3.X - self.P1.X))
                newy = self.P2.Y                
                pTmp = Point( newx,newy )
                fillBottomFlatTriangle(self.P1, self.P2, pTmp)
                fillTopFlatTriangle(self.P2, pTmp, self.P3)

def fillBottomFlatTriangle(p1,p2,p3):
    slope1 = float(p2.X - p1.X) / float (p2.Y - p1.Y)
    slope2 = float(p3.X - p1.X) / float (p3.Y - p1.Y)

    x1 = p1.X
    x2 = p1.X + 0.5

    for scanlineY in range(p1.Y,p2.Y):
#        lcd.pixel_span(int(x1), scanlineY, int(x2)-int(x1))   # Switch pixel_span() to hline() / Pimoroni to WS
        fbuff.hline(int(x1),scanlineY, int(x2)-int(x1),1)
        ##fbuff.display()
        utime.sleep(0.1)
        x1 += slope1
        x2 += slope2

def fillTopFlatTriangle(p1,p2,p3):
    slope1 = float(p3.X - p1.X) / float(p3.Y - p1.Y)
    slope2 = float(p3.X - p2.X) / float(p3.Y - p2.Y)

    x1 = p3.X
    x2 = p3.X + 0.5

    for scanlineY in range (p3.Y,p1.Y-1,-1):
#        fbuff.pixel_span(int(x1), scanlineY, int(x2)-int(x1))  # Switch pixel_span() to hline() / Pimoroni to WS
        fbuff.hline(int(x1),scanlineY, int(x2)-int(x1),1)
        #fbuff.display()
        utime.sleep(0.1)
        x1 -= slope1
        x2 -= slope2
            

#empty triangle 
def triangle(x1,y1,x2,y2,x3,y3,c): # Draw outline triangle (empty)
    fbuff.line(x1,y1,x2,y2,c)
    fbuff.line(x2,y2,x3,y3,c)
    fbuff.line(x3,y3,x1,y1,c)
#filled triangle 
def fill_triangle(x1,y1,x2,y2,x3,y3,c): # Draw filled triangle
    t=Triangle(Point(x1,y1),Point(x2,y2),Point(x3,y3)) # Define corners
    t.fillTri()

# ============== End of Triangles Code ===============
#================ Text scrolling =====================
#oled width is 128(x) , oled hight 64(y)
def scroll(text,y):
  for i in range (0, (129)*2, 1):
    fbuff.text(text, -128+i, y,1)
    show()
    if i!= 128:
      fill(0)

setupCMD = (
    SET_DISP | 0x00,  # off
    # address setting
    SET_MEM_ADDR, 0x00,  # horizontal
    # resolution and layout
    SET_DISP_START_LINE | 0x00,
    SET_SEG_REMAP | 0x01,  # column addr 127 mapped to SEG0
    SET_MUX_RATIO, 64 - 1,
    SET_COM_OUT_DIR | 0x08,  # scan from COM[N] to COM0
    SET_DISP_OFFSET, 0x00,
    SET_COM_PIN_CFG, 0x12,
    # timing and driving scheme
    SET_DISP_CLK_DIV, 0x80,
    SET_PRECHARGE, 0xF1,
    SET_VCOM_DESEL, 0x30,  # 0.83*Vcc
    # display
    SET_CONTRAST, 0xFF,  # maximum
    SET_ENTIRE_ON,  # output follows RAM contents
    SET_NORM_INV,  # not inverted
    SET_CHARGE_PUMP, 0x14, # charge pump
    SET_DISP | 0x01, # on
)

for cmd in setupCMD:  
    write_cmd(cmd)
fill(0)
show()

