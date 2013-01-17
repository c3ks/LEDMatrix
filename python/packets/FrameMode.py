import array

class FrameMode(object):
    
    def __init__(self):
        self.__importMode = 0x00
        self.__exportMode = 0x00
        self.__delay = 0x00
        self.__speed = 0x07
        self.__appendMode = 0x00
        self.__animation = 0x00
    
    def __del__(self):
        pass
    
    def toCharArray(self):
        #return chars here
        data = array.array('c')
        data.append(chr(self.__importMode))
        data.append(chr(self.__exportMode))
        data.append(chr(self.__delay))
        data.append(chr(self.__speed))
        data.append(chr(self.__appendMode))
        data.append(chr(self.__animation))
        
        return data
        
    
    def setImportMode(self, mode):
        #0x00 till 0x0F
        self.__importMode = mode
        
    def setExportMode(self, mode):
        self.__exportMode = mode
        
    def setDelay(self, time):
        self.__delay = time
        
    def setSpeed(self, speed):
        self.__speed = speed
        
    def setAppendMode(self, mode):
        self.__appendMode = mode

    def setAnimation(self, mode):
        self.__animation = mode