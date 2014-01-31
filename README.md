# LED Matrix Projects
Drivers and Stuff for our (~~RS232~~ RJ45 controlled) LED Matrix

### Perl code (not working) - Stephan Oeste (emzy.de)
Reverse engineering of original protocol (windows software). 
First attempt to replace the default software
### Python code - Stefan Triller (stefantriller.de)
Still working code to replace the default software. It's possible to send multi colored text to the LED panel
### Java code - Sven Steenbock
It's not a tool to send text to the panel, but to receive it. 
### LED-Panel-Controller - Sven Steenbock
Sven managed to completely reverse engineer the old controller of the LED panel and replace it by his own to provide ethernet including DHCP instead of rs232. The software also allows us to send pure text or binary data to the panel.

## Tools & Games
### LED Snake -  Enno Boland (gottox.de) 
LED Snake is a clone of the good old arcade / nokia game also called Snake. It uses the whole LED panel and also supports levels with walls. The game runs on the local computer, renders the full view and sends it to the panel.
### node-ledpanel - Enno Boland (gottox.de)
Enno built a JavaScript (node.js) library to send stuff to the LED panel. It makes developing for the panel way easier, since you can simply draw stuff on a canvas element and send it to the panel. Project can be found in a [seperate repository](https://github.com/c3ks/node-ledpanel)

