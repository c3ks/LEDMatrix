import array

class FirstRedPacket(object):

    def __init__(self):
        self.__head = 0x7F
    
    def setPayLoad(self, payLoad):
        #val = DataHelper.DataHelper.dec2hex(num)
        #self.__numFrames = val
        self.__payLoad = payLoad
    
    def toCharArray(self):
        data = array.array('c')
        
        data.append(chr(self.__head))
        
        for i in range(9):
            data.append(chr(0x00))
            
        data.extend(self.__payLoad)
        
        return data