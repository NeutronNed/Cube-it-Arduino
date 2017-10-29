# Author: Kristina Durivage based on the uart service demo by Tony DiCola
import os
import Adafruit_BluefruitLE
import time
import random
import re
from Adafruit_BluefruitLE.services import UART

ble = Adafruit_BluefruitLE.get_provider()

# Not a fluent python dev - No idea if this is the best way to deal with global variables or not
runTest = True
ts = time.time()

def main():
    global runTest
    global ts
    global lastSideNum
    ble.clear_cached_data()

    # Get the first available BLE network adapter and make sure it's powered on.
    adapter = ble.get_default_adapter()
    adapter.power_on()
    print('Using adapter: {0}'.format(adapter.name))

    print('Disconnecting any connected UART devices...')
    UART.disconnect_devices()
    
    print('Searching for Bluetooth UART device...')
    try:
        adapter.start_scan()
        # TODO: Can I connect to a specific device?? This doesn't work obviously :p
        device = UART.find_device('EB:0A:91:D4:5E:85fghdfgdfg')
        if device is None:
            raise RuntimeError('Failed to find UART device!')
    finally:
        # Make sure scanning is stopped before exiting.
        adapter.stop_scan()

    print('Connecting to device...')
    device.connect()  # Will time out after 60 seconds, specify timeout_sec parameter
                      # to change the timeout.

    try:
        # Wait for service discovery to complete for the UART service.  Will
        # time out after 60 seconds (specify timeout_sec parameter to override).
        UART.discover(device)
        
        uart = UART(device)
        
        command(uart)
        
        whitespacePattern = re.compile(r'\s+')
        
        # goals for getting the input stream a bit more formatted, not used yet
        class accelReading:
            x = ''
            y = ''
            z = ''
                
        class buttonReading:
            side = ''
            press = ''
        
        pastMsg = []
        
        while runTest :
            received = uart.read(timeout_sec=60)
                
            # the timeout code, if we dont issue a new command within 2 minutes, stop the test    
            if (time.time() - ts) >= (60 * 2):
                runTest = False
            
            if received is not None:
                #remove line breaks
                received = re.sub(whitespacePattern, '', received)
                if received is not ',':
                    print('Received: {0}'.format(received))
                    receivedArr = received.split(',')
                    
                    #if theres a past message, append it to whats recieved
                    if len(pastMsg) > 0:
                        receivedArr = pastMsg + receivedArr
                    
                    #if there is no S in this msg or the past msg, clear out the past
                    if 'S' not in receivedArr:
                        pastMsg = []
                    
                    if len(receivedArr) > 3:
                        for idx, receivedBit in enumerate(receivedArr):
                            if receivedBit is 'S':
                                # if the recievedArr doesn't have two numbers after the S, 
                                if len(receivedArr) <= (idx + 2):
                                    pastMsg = pastMsg + receivedArr
                                else:
                                    pastMsg = [] #clear out on valid side press
                                    print('side pressed ' + receivedArr[idx+1] + ' correct ' + lastSideNum)
                                    if receivedArr[idx+1] is lastSideNum: # and receivedArr[idx+2] is '1':
                                        newTime = time.time();
                                        print('hit correct side in ' + str(newTime - ts))
                                        ts = newTime
                                        command(uart)
                    else:
                        pastMsg = pastMsg + receivedArr; # keep very small received strs in array
            else:
                print('Received no data, is your device on?')
                break;
    finally:
        # Make sure device is disconnected on exit.
        print('test over!');
        device.disconnect()

def command(uart):
    global ts
    global lastSideNum
    # choose a random color
    colorNum = str(random.randint(1,4))
    colorName = ''
    if colorNum is '0':
        colorName = 'off'
    elif colorNum is '1':
        colorName = 'red'
    elif colorNum is '2':
        colorName = 'green'
    elif colorNum is '3':
        colorName = 'blue'
    elif colorNum is '4':
        colorName = 'white'

    # choose a random side
    sideNum = str(random.randint(0,5))
    
    lastSideNum= sideNum
    
    print('Sending F,'+ sideNum + ',' + colorNum + '\r\n');
    
    uart.write('F,'+ sideNum + ',' + colorNum + '\r\n')
    ts = time.time()
    # voice command
    os.system('flite -voice slt -t "Press the ' + colorName + ' button!" ')


ble.initialize()

# Start the mainloop to process BLE events, and run the provided function in
# a background thread.  When the provided main function stops running, returns
# an integer status code, or throws an error the program will exit.
ble.run_mainloop_with(main)
