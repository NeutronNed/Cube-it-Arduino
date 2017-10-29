# Example of interaction with a BLE UART device using a UART service
# implementation.
# Author: Kristina Durivage based on the uart service demo by Tony DiCola
import os
import Adafruit_BluefruitLE
import time
import random
from Adafruit_BluefruitLE.services import UART

ble = Adafruit_BluefruitLE.get_provider()
runTest = True
ts = time.time()

def main():
    global runTest
    global ts
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
        # TODO: Can I connect to a specific device?? 
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
        
        # read continuously
        while runTest :
            received = uart.read(timeout_sec=60)
            
            if (time.time() - ts) >= (60 * 2):
                runTest = False
            
            if received is not None:
                print('Received: {0}'.format(received))
            else:
                print('Received no data, is your device on?')
    finally:
        # Make sure device is disconnected on exit.
        print('test over!');
        device.disconnect()

def command(uart):
    global ts
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
    elif colorName is '4':
        colorName = 'white'

    # choose a random side
    sideNum = str(random.randint(0,6))
    
    print('F,'+ sideNum + ',' + colorNum + '\r\n');
    
    uart.write('F,'+ sideNum + ',' + colorNum + '\r\n')
    ts = time.time()
    # voice command
    os.system('flite -t "Press the ' + colorName + 'button!" ')


ble.initialize()

# Start the mainloop to process BLE events, and run the provided function in
# a background thread.  When the provided main function stops running, returns
# an integer status code, or throws an error the program will exit.
ble.run_mainloop_with(main)
