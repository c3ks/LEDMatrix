import array

class DataHelper(object):
    
    def fillTo1000(data):
        if len(data) < 1000:
            for x in range(1000-len(data)):
                data.append(chr(0x00))
                
        return data
 
    def printBytes(data):               
        for b in data:
            str = "0x"
            if ord(b) < 0x0F:
                str += "0"
            
            str = str + "%X" %(ord(b))
            print str
            #print "0x%X" %(ord(b))
        
        print "Byte Count: %d" % (len(data))
  
    def dec2hex(n):
        """return the hexadecimal string representation of integer n"""
        ret = "0x"
        if n<16:
            ret += "0"
        
        ret = ret + "%X" %n
        
        return ret
        #return "0x%X" % n
 
    def hex2dec(s):
        """return the integer value of a hexadecimal string s"""
        return int(s, 16)
    
    dec2hex = staticmethod(dec2hex)
    
    hex2dec = staticmethod(hex2dec)
 
    fillTo1000 = staticmethod(fillTo1000)
    
    printBytes = staticmethod(printBytes)