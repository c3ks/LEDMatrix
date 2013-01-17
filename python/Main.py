import gd
import os
import array

from connection import *
from helper import *
from packets import *

os.environ["GDFONTPATH"] = "."

sp = SerialPort.SerialPort()

gd.gdMaxColors = 1

img = gd.image((128, 16))

tuximg=gd.image((128,16))

black = img.colorAllocate((0, 0, 0))
white = img.colorAllocate((255, 255, 255))

tuximg.setPixel((0,0), white)
tuximg.setPixel((0,1), white)
tuximg.setPixel((1,1), white)
tuximg.setPixel((2,1), white)
tuximg.setPixel((3,1), white)
tuximg.setPixel((4,1), white)
tuximg.setPixel((5,8), white)
tuximg.setPixel((6,8), white)
tuximg.setPixel((7,8), white)
tuximg.setPixel((8,8), white)
tuximg.setPixel((9,8), white)
tuximg.setPixel((10,8), white)
tuximg.setPixel((11,8), white)
tuximg.setPixel((12,8), white)
tuximg.setPixel((13,8), white)

tuximg.setPixel((13,15), white)

tuximg.setPixel((127,15), white)

#img.setPixel((0,0), white)

#img.setPixel((127,15), white)

#img.string_ttf(font, pointsize, angle, (x,y), s, color)

img.string_ttf("cour", 10, 0, (0,12), "HALLO CCC Kassel", white)

#img.gdMaxColors(256)

#img.rectangle((2,2),(10,10),white)

#test = StringIO.StringIO()

#test.write("lala")

bild=open("xx.bmp","w")
#img.writePng(bild)
#img.writeGd2(test)
#img.writeGd(f)
img.writeWbmp(bild) # chop the first 5 bytes (bmp-header)
#tuximg.writeWbmp(bild) # chop the first 5 bytes (bmp-header)

bild.close()

def sendFile(file):
    print file
    
    f = open(file,"rb")
    str = f.read(1000)
    #while (byte = f.read(1)) != EOF:
    #for j in range(1000):
    #for byte in f.read(1):
        #print "%x" %(byte)
        
    #for b in range(0,len(str)):
    #   print "0x%X" %(ord(str[b]))
        
    f.close()
    
    #str.
    
    sp.sendData(0, str)

#main

#build command packet
cp = CommandPacket.CommandPacket()

#cp.setNumFrames(6)
cp.setNumFrames(1)

cp.setFirstFrame(0)

#cp.setLastFrame(5)
cp.setLastFrame(0)

cp.setD0(1)

fm = FrameMode.FrameMode()

fm.setImportMode(0)
fm.setExportMode(0)
fm.setDelay(3)

#fm.setSpeed(7)
fm.setSpeed(4)

fm.setAppendMode(64)
#fm.setAppendMode(128) #0 1 4 8 16 64 128
fm.setAnimation(0)

cp.attachFrameMode(fm)

commandData = cp.toCharArray()

DataHelper.DataHelper.printBytes(commandData)

commandData = DataHelper.DataHelper.fillTo1000(commandData)

sp.sendData(0, commandData.tostring())

#file 2
#fileName = "2.bin"
#sendFile(fileName)

#picture red

data = array.array('c')

data.append(chr(0x7f))

for i in range(9):
    data.append(chr(0x00))

picFile = open("xx.bmp","rb")
#picData = picFile.read()

picFile.seek(5) #skip bmp header

picData = array.array('c')

try:
    picData.fromfile(picFile, 989)
except EOFError:
    print "Less data"

picData.byteswap() #reverse byteorder for display

data.extend(picData)

#print "cur len %d" %(len(data))

data = DataHelper.DataHelper.fillTo1000(data)

DataHelper.DataHelper.printBytes(data)

sp.sendData(0, data.tostring())

#picture green

data = array.array('c')

data.append(chr(0x6f))

for i in range(9):
    data.append(chr(0x00))

picFile = open("xx.bmp","rb")
#picData = picFile.read()

picFile.seek(5) #skip bmp header

picData = array.array('c')

try:
    picData.fromfile(picFile, 989)
except EOFError:
    print "Less data"

picData.byteswap() #reverse byteorder for display

data.extend(picData)

#print "cur len %d" %(len(data))

data = DataHelper.DataHelper.fillTo1000(data)

DataHelper.DataHelper.printBytes(data)

sp.sendData(0, data.tostring())

#file 4 and 5
for i in range(3,5):
 #   print i
    
    fileName = "%d.bin" % (i+1)    
    sendFile(fileName)