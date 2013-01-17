import serial
import gd
import OurPicture

import StringIO

def buildConnectionString(params):
    """Build a connection string from a dictionary of parameters.

    Returns string."""
    #tmp = [(k, v) for k, v in params.items()]
    
    tmp = ""
    
    for k, v in params.items():
        #tmp += " | %s=%s" % (k, v)
        tmp += " | " + str((k,v))
    
    #return tmp
    return " | ".join(["%s=%s" % (k, v) for k, v in params.items()])

if __name__ == "__main__":
    myParams = {"server":"mpilgrim",
                "database":"master",
                "uid":"sa",
                "pwd":"secret"
                }

#ser = serial.Serial()

#ser.bytesize = 8

#print ser

li = [1,2,3]
li2 = [4,5,6]

for x in li2:
    li.append(x)

[li.append(x) for x in li2]

print li

print buildConnectionString(myParams)

gd.gdMaxColors = 1

img = gd.image((128, 16))

black = img.colorAllocate((0, 0, 0))
white = img.colorAllocate((255, 255, 255))

#img.gdMaxColors(256)

img.rectangle((2,2),(10,10),white)

print "hallo"

test = StringIO.StringIO()

test.write("lala")

f=open("xx.bmp","w")
#img.writePng(test)
#img.writeGd2(test)
#img.writeGd(f)
img.writeGd2(f, 0, gd.GD2_FMT_RAW)
#img.writeJpeg(test)

#img.writeWbmp(OurPicture)
#img.writeWbmp(test)
f.close()
print test.len
print test.getvalue()
#print img
