import array

import gd
import os

os.environ["GDFONTPATH"] = "."
gd.gdMaxColors = 1

class Text2PictureHelper(object):
    
    #def __init__(self):
        
        
    
    def generatePic(text):
        
        data = array.array('c')
        img = gd.image((128, 16))
        
        white = img.colorAllocate((255, 255, 255))
        
        img.string_ttf("cour", 14, 0, (0,12), text, white)
        
        pic=open("xx.bmp","w")
        img.writeWbmp(pic)        
        pic.close()
        
        picFile = open("xx.bmp","rb")

        picFile.seek(5) #skip bmp header
        
        try:
            data.fromfile(picFile, 989)
        except EOFError:
            print "Less data"

        data.byteswap()
        
        return data
    
    generatePic = staticmethod(generatePic)