import array

from helper import DataHelper

class CommandPacket(object):

    def __init__(self):
        self.__head = 0x5F
        self.__numFrames = 0x00
        self.__nil = 0x00
        self.__firstFrame = 0x00
        self.__lastFrame = 0x00
        self.__signLength = 0x10
        self.__signHeight = 0x02
        self.__colorMode = 0x02 #tri color
        self.__maxFrames = 0x96 #150d
        self.__d0 = 0x00
        self.__frameModes = []
    
    def setNumFrames(self, num):
        #val = DataHelper.DataHelper.dec2hex(num)
        #self.__numFrames = val
        self.__numFrames = num
        
    def setFirstFrame(self, num):
        self.__firstFrame = num
        
    def setLastFrame(self, num):
        self.__lastFrame = num
        
    def setColorMode(self, num):
        self.__colorMode = num
        
    def setMaxFrames(self, num):
        self.__maxFrames = num
        
    def setD0(self, num):
        self.__d0 = num
        
    def attachFrameMode(self, fm):
        self.__frameModes.append(fm)        
    
    def toCharArray(self):
        data = array.array('c')
        data.append(chr(self.__head))
        data.append(chr(self.__numFrames))
        data.append(chr(self.__nil))
        data.append(chr(self.__firstFrame))
        data.append(chr(self.__lastFrame))
        data.append(chr(self.__signLength))
        data.append(chr(self.__signHeight))
        data.append(chr(self.__colorMode))
        data.append(chr(self.__maxFrames))
        data.append(chr(self.__d0))
        
        for fm in self.__frameModes:
            for b in fm.toCharArray():
                data.append(b)
        
        return data
        
        
    
    