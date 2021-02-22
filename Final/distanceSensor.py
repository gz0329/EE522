#!/usr/bin/env python

import time
import smbus
#from smbus2 import SMBus

BL_CMD_STAT = 0x80
ENABLE = 0xe0
APPREQID = 0x02
CMD_DATA7 = 0x08

bus = smbus.SMBus(1)

# default addr of distance sensor
addr = 0x41

def write_byte(reg, val):
    bus.write_byte_data(addr, reg, val)

def read_byte(reg):
    result = bus.read_byte_data(addr, reg)
    return result

def write_bytes(reg, val):
    bus.write_i2c_block_data(addr, reg, val)

def read_bytes(reg, size):
    result = bus.read_i2c_block_data(addr, reg, size)
    return result

write_byte(ENABLE, 0b10000001)

print(hex(read_byte(0x00)))

write_byte(APPREQID, 0xC0)

#write_byte(ENABLE, 0b10000001)
time.sleep(1)
print(hex(read_byte(0x00)))

# use example cali and state data
cali = [0x01, 0x17, 0x00, 0xff, 0x04, 0x20, 0x40, 0x80, 0x00, 0x01, 0x02, 0x04, 0x00, 0xfc]
state = [0xb1, 0xa9, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00]
write_bytes(0x20, cali)
write_bytes(0x2e, state)
time.sleep(2)

continous_100ms = [0x03, 0x23, 0x00, 0x00, 0x00, 0x64, 0xd8, 0x04, 0x02]
write_bytes(CMD_DATA7, continous_100ms)
time.sleep(1)
print( 'status' + str(read_byte(0x1d)))

print( 'content' + str(read_byte(0x1e)))

while( 1 ):
    #print( 'content' + str(read_byte(0x1e)))
    result = str(read_bytes(0x21,10))
    print( 'Distance peaks: ' + str(read_bytes(0x22,2)))
    time.sleep(2)
#print(read_byte(0x20))