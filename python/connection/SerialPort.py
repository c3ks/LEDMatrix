import serial
import time

class SerialPort(object):
    
    #__port
    
    def __init__(self):
        self.__port = serial.Serial()
        self.__port.baudrate = 19200
        self.__port.port = "/dev/ttyUSB0"
        self.__port.stopbits = 1
        self.__port.bytesize = 8
        self.__port.parity = serial.PARITY_NONE
        self.__port.timeout = 0.1 #100 ms
        self.__port.writetimeout = None
        
        self.__port.open()
        
    def __del__(self):
        self.__port.close()
        
        #self.__numDisplay = 0x00

    def sendData(self, numDisplay, bytes):
        #__port.write() ...
        
        if numDisplay % 2 == 0:
            print "richtig"
            self.__port.parity = serial.PARITY_ODD
        else:
            print "falsch"
            self.__port.parity = serial.PARITY_EVEN
            
        self.__port.write(chr(numDisplay))
        
        time.sleep(0.3)
        #toggle parity bit on first byte of bytes
        
        if ord(bytes[0]) % 2:
            print "set to odd"
            self.__port.parity = serial.PARITY_ODD
        else:
            print "set to even"
            self.__port.parity = serial.PARITY_EVEN

        time.sleep(0.3)
        
        sum = 0
        for b in bytes:    
            sum += ord(b)
            
        sum %= 256
        print "Checksum: 0x%X" % (sum)

        bytes += chr(sum)
        
        tmp = 0
        for c in bytes:
            
#            print "%d: 0x%X" %(tmp, ord(c))
            tmp += 1
            self.__port.write(c)
#        self.__port.write(bytes)       

        print "%d bytes" %(tmp)

        #time.sleep(0.5)
        
        