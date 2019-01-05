#!/usr/bin/python
# Raspberry Pi OTP Dump Parser
#
# Copyright 2019 Jasmine Iwanek & Dylan Morrison
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of this
# software and associated documentation files (the "Software"), to deal in the Software
# without restriction, including without limitation the rights to use, copy, modify, merge,
# publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons
# to whom the Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all copies or
# substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED *AS IS*, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
# PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
# FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
# OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# DEALINGS IN THE SOFTWARE.
#
# Usage
# call either ./OTPParser.py <filename> or vgcencmd otp_dump | OTPParser
#
from __future__ import print_function
from bitstring import BitArray
import sys
import os.path

memory_sizes = {
    '256':       '000', # 0
    '512':       '001', # 1
    '1024':      '010', # 2
    'unknown_3': '011', # 3
    'unknown_4': '100', # 4
    'unknown_5': '101', # 5
    'unknown_6': '110', # 6
    'unknown_7': '111', # 7
    'Unknown': ''
}
memory_sizes_asString = {v: k for k, v in memory_sizes.items()}

manufacturers = {
    'Sony UK':    '0000', # 0
    'Egoman':     '0001', # 1
    'Embest':     '0010', # 2
    'Sony Japan': '0011', # 3
    'Embest #2':  '0100', # 4
    'Stadium':    '0101', # 5
    'Unknown_6':  '0110', # 6
    'Unknown_7':  '0111', # 7
    'Unknown_8':  '1000', # 8
    'Unknown_9':  '1001', # 9
    'Unknown_a':  '1010', # a
    'Unknown_b':  '1011', # b
    'Unknown_c':  '1100', # c
    'Unknown_d':  '1101', # d
    'Unknown_e':  '1110', # e
    'Unknown_f':  '1111', # f
    'Qisda':      'QISD', # TODO: Correct value unknown
    'Unknown':    ''
}
manufacturers_asString = {v: k for k, v in manufacturers.items()}

processors = {
    'BCM2835':   '0000', # 0
    'BCM2836':   '0001', # 1
    'BCM2837':   '0010', # 2
    'Unknown_3': '0011', # 3
    'Unknown_4': '0100', # 4
    'Unknown_5': '0101', # 5
    'Unknown_6': '0110', # 6
    'Unknown_7': '0111', # 7
    'Unknown_8': '1000', # 8
    'Unknown_9': '1001', # 9
    'Unknown_a': '1010', # a
    'Unknown_b': '1011', # b
    'Unknown_c': '1100', # c
    'Unknown_d': '1101', # d
    'Unknown_e': '1110', # e
    'Unknown_f': '1111', # f
    'Unknown':   ''

}
processors_asString = {v: k for k, v in processors.items()}

# TODO: This can cause crashes when passed values between 10 and ff
board_types = {
    'A':          '00000000', # 0
    'B':          '00000001', # 1
    'A+':         '00000010', # 2
    'B+':         '00000011', # 3
    '2B':         '00000100', # 4
    'Alpha':      '00000101', # 5
    'CM1':        '00000110', # 6
    'unknown_7':  '00000111', # 7 (Not in known use)
    '3B':         '00001000', # 8
    'Zero':       '00001001', # 9
    'CM3':        '00001010', # a
    'unknown_b':  '00001011', # b (Not in known use)
    'Zero W':     '00001100', # c
    '3B+':        '00001101', # d
    '3A+':        '00001110', # e
    'Unknown_f':  '00001111', # f (Not in known use)
    'Unknown':   ''
}
board_types_asString = {v: k for k, v in board_types.items()}

board_revisions = {
    '1.0':     '0000',
    '1.1':     '0001',
    '1.2':     '0010',
    '1.3':     '0011',
    'unk_4':   '0100',
    'unk_5':   '0101',
    'unk_6':   '0110',
    'unk_7':   '0111',
    'unk_8':   '1000',
    'unk_9':   '1001',
    'unk_a':   '1010',
    'unk_b':   '1011',
    'unk_c':   '1100',
    'unk_d':   '1101',
    'unk_e':   '1110',
    'unk_e':   '1110',
    'unk_f':   '1111',
    '2.0':     ' 2.0', # TODO: Correct Value unknown.
    'unknown': ''
}
board_revisions_asString = {v: k for k, v in board_revisions.items()}

regions = {
    'unknown_8':               8,
    'unknown_9':               9,
    'unknown_10':             10,
    'unknown_11':             11,
    'unknown_12':             12,
    'unknown_13':             13,
    'unknown_14':             14,
    'unknown_15':             15,
    'unknown_16':             16, # Usually 00280000, may? change to 242800 when you enable access to i2c2, as yet unknown
    'bootmode':               17, # BootMode Register
    'bootmode_copy':          18, # Backup copy of BootMode Register
    'unknown_19':             19, # Always ffffffff in tests
    'unknown_20':             20, # Always ffffffff in tests
    'unknown_21':             21, # Always ffffffff in tests
    'unknown_22':             22, # Always ffffffff in tests
    'unknown_23':             23, # Always ffffffff in tests
    'unknown_24':             24, # Always ffffffff in tests
    'unknown_25':             25, # Always ffffffff in tests
    'unknown_26':             26, # Always ffffffff in tests
    'unknown_27':             27, # Has varying values, as yet unknown
    'serial_number':          28, # Serial Number
    'serial_number_inverted': 29, # Serial Number (Bitflipped)
    'revision_number':        30, # Revision Number
    'batch_number':           31, # Batch Number
    'overclock':              32, # Overclock Register
    'unknown_33':             33,
    'unknown_34':             34,
    'unknown_35':             35,
    'customer_one':           36, # 
    'customer_two':           37, # 
    'customer_three':         38, # 
    'customer_four':          39, # 
    'customer_five':          40, # 
    'customer_six':           41, # 
    'customer_seven':         42, # 
    'customer_eight':         43, # 
    'unknown_44':             44,
    'codec_key_one':          45, # Codec License Key #1
    'codec_key_two':          46, # Codec License Key #2
    'unknown_47':             47,
    'unknown_48':             48,
    'unknown_49':             49,
    'unknown_50':             50,
    'unknown_51':             51,
    'unknown_52':             52,
    'unknown_53':             53,
    'unknown_54':             54,
    'unknown_55':             55,
    'unknown_56':             56,
    'unknown_57':             57,
    'unknown_58':             58,
    'unknown_59':             59,
    'unknown_60':             60,
    'unknown_61':             61,
    'unknown_62':             62,
    'unknown_63':             63,
    'mac_address_two':        64, # MAC Address (Second part)
    'mac_address_one':        65, # MAC Address (First part)
    'advanced_boot':          66  # Advanced Boot Register
}

memory_size = '000'
manufacturer = '0000'
processor = '0000'
board_type = '00000000'
board_revision = '0000'

data = []

def unknown_16(name):
    indices = {
            'bits_0_to_15':  ( 16, 32),
            'bits_16_to_23': ( 8,  16), # only 0x28 seen thus far
            'bits_24_to_31': ( 0,  8)  # 0x24 & 0x6c seen here thus far
    }[name]
    unknown_16 = getBinary('unknown_16')
    return unknown_16[indices[0] : indices[1]]

def bootmode(name):
    indices = {
        'bit_0':         (31, 32), # Unknown (Gordon hinted the Pi wouldn't boot with this set)
        'bit_1':         (30, 31), # Sets the oscillator frequency to 19.2MHz
        'bit_2':         (29, 30), # Unknown (Gordon hinted the Pi wouldn't boot with this set)
        'bit_3':         (28, 29), # Enables pull ups on the SDIO pins
        'bits_4_to_18':  (13, 28), # Unknown/Unused
        'bit_19':        (12, 13), # Enables GPIO bootmode
        'bit_20':        (11, 12), # Sets the bank to check for GPIO bootmode
        'bit_21':        (10, 11), # Enables booting from SD card
        'bit_22':        ( 9, 10), # Sets the bank to boot from (That's what Gordon said, I think it means SD Card though)
        'bits_26_to_27': ( 7,  9), # Unknown/Unused
        'bit_25':        ( 6,  7), # Unknown (Is set on the Compute Module 3)
        'bits_23_to_24': ( 4,  6), # Unknown/Unused
        'bit_28':        ( 3,  4), # Enables USB device booting
        'bit_29':        ( 2,  3), # Enables USB host booting (Ethernet and Mass Storage)
        'bits_30_31':    ( 0,  2)  # Unknown/Unused
    }[name]
    bootmode = getBinary('bootmode')
    return bootmode[indices[0] : indices[1]]

def unknown_27(name):
    indices = {
            'bits_0_to_15':  (16, 32), # 5050 (1B, 2B 1.1), 7373 (2B 1.2), 2727(CM3), 1f1f (3B+)
            'bits_16_to_31': ( 0, 16)
    }[name]
    unknown_27 = getBinary('unknown_27')
    return unknown_27[indices[0] : indices[1]]

def revision(name):
    indices = {
        'legacy_board_revision': (27, 32), # Region used to store the legacy revision, only use if new_flag is not set. Note: it's actually 8 bits, but only 5 were ever used
        'board_revision':        (28, 32), # Revision of the board
        'board_type':            (20, 28), # Model of the board
        'processor':             (16, 20), # Installed Processor
        'manufacturer':          (12, 16), # Manufacturer of the board
        'memory_size':           ( 9, 12), # Amount of RAM the board has
        'new_flag':              ( 8,  9), # If set, this board uses the new versioning scheme
        'bits_24_to_31':         ( 0,  8)  # Unused
    }[name]
    revision = getBinary('revision_number')
    return revision[indices[0] : indices[1]]

def overclock(name):
    indices = {
        'overvolt_protection': (31, 32), # Overvolt protection bit
        'bits_0_to_30':        ( 0, 31)  # Unknown/Unused
    }[name]
    overclock = getBinary('overclock')
    return overclock[indices[0] : indices[1]]

def advanced_boot(name):
    indices = {
        'bits_0_to_6':   (25, 32), # GPIO for ETH_CLK output pin
        'bit_7':         (24, 25), # Enable ETH_CLK output pin
        'bits_8_to_14':  (17, 24), # GPIO for LAN_RUN output pin
        'bit_15':        (16, 17), # Enable LAN_RUN output pin
        'bits_16_to_23': ( 8, 16), # Unknown/Unused
        'bit_24':        ( 7,  8), # Extend USB HUB timeout parameter
        'bit_25':        ( 6,  7), # ETH_CLK Frequency (0 = 25MHz, 1 = 24MHz)
        'bits_26_to_31': ( 0,  6)  # Unknown/Unused
    }[name]
    advanced_boot = getBinary('advanced_boot')
    return advanced_boot[indices[0] : indices[1]]

def processBootMode():
    bootmode_primary = getBinary('bootmode')
    bootmode_copy = getBinary('bootmode_copy')
    if bootmode_primary != bootmode_copy:
        print('Bootmode fields are not the same, this is a bad thing!')

def testBit(int_type, offset):
    return(int_type & mask)

def processSerial():
    serial = getHex('serial_number')
    inverse_serial = getHex('serial_number_inverted')
    if ((serial ^ inverse_serial) != '0xffffffff'):
        print('Serial failed checksum!')

def processRevision():
    flag = revision('new_flag')
    if flag == '0':
        generateInfoLegacy(revision('legacy_board_revision'))
    elif flag == '1':
        generateInfo(revision('memory_size'), revision('manufacturer'), revision('processor'), revision('board_type'), revision('board_revision'))

def formatMAC():
    mac_part_1 = getData('mac_address_one')
    mac_part_2 = getData('mac_address_two')
    if not mac_part_1 == '00000000':
        y = mac_part_1 + mac_part_2
        return ':'.join(y[i:i+2] for i in range(0,12,2))
    else:
        return 'None'

def processABHubTimeout(x):
    if x == '1':
        return '5 Seconds'
    else:
        return '2 Seconds'

def processABETH_CLKFrequency(x):
    if x == '1':
        return '24MHz'
    else:
        return '25MHz'

def generateInfoLegacy(x):
    if x == '00000':
        generateInfoFromDict('unknown', 'unknown', 'unknown', 'unknown', 'unknown'), # 00 - Unknown Model
    elif x == '00001':
        generateInfoFromDict('unknown', 'unknown', 'unknown', 'unknown', 'unknown'), # 01 - Unknown Model
    elif x == '00010':
        generateInfoFromDict('256',     'Egoman',  'BCM2835', 'B',       '1.0'    ), # 02
    elif x =='00011':
        generateInfoFromDict('256',     'Egoman',  'BCM2835', 'B',       '1.0'    ), # 03
    elif x == '00100':
        generateInfoFromDict('256',     'Sony UK', 'BCM2835', 'B',       '2.0'    ), # 04
    elif x == '00101':
        generateInfoFromDict('256',     'Qisda',   'BCM2835', 'B',       '2.0'    ), # 05
    elif x == '00110':
        generateInfoFromDict('256',     'Egoman',  'BCM2835', 'B',       '2.0'    ), # 06
    elif x == '00111':
        generateInfoFromDict('256',     'Egoman',  'BCM2835', 'A',       '2.0'    ), # 07
    elif x == '01000':
        generateInfoFromDict('256',     'Sony UK', 'BCM2835', 'A',       '2.0'    ), # 08
    elif x == '01001':
        generateInfoFromDict('256',     'Qisda',   'BCM2835', 'A',       '2.0'    ), # 09
    elif x == '01010':
        generateInfoFromDict('unknown', 'unknown', 'unknown', 'unknown', 'unknown'), # 0a - Unknown Model
    elif x == '01011':
        generateInfoFromDict('unknown', 'unknown', 'unknown', 'unknown', 'unknown'), # 0b - Unknown Model
    elif x == '01100':
        generateInfoFromDict('unknown', 'unknown', 'unknown', 'unknown', 'unknown'), # 0c - Unknown Model
    elif x == '01101':
        generateInfoFromDict('512',     'Egoman',  'BCM2835', 'B',       '2.0'    ), # 0d
    elif x == '01110':
        generateInfoFromDict('512',     'Sony UK', 'BCM2835', 'B',       '2.0'    ), # 0e
    elif x == '01111':
        generateInfoFromDict('512',     'Egoman',  'BCM2835', 'B',       '2.0'    ), # 0f
    elif x == '10000':
        generateInfoFromDict('512',     'Sony UK', 'BCM2835', 'B+',      '1.0'    ), # 10
    elif x == '10001':
        generateInfoFromDict('512',     'Sony UK', 'BCM2835', 'CM1',     '1.0'    ), # 11
    elif x == '10010':
        generateInfoFromDict('512',     'Sony UK', 'BCM2835', 'A+',      '1.1'    ), # 12
    elif x == '10011':
        generateInfoFromDict('512',     'Embest',  'BCM2835', 'B+',      '1.2'    ), # 13
    elif x == '10100':
        generateInfoFromDict('512',     'Embest',  'BCM2835', 'CM1',     '1.0'    ), # 14
    elif x == '10101':
        generateInfoFromDict('512',     'Embest',  'BCM2835', 'A+',      '1.1'    ), # 15 - TODO: This can actually be 256MB or 512MB.
    elif x == '10110':
        generateInfoFromDict('unknown', 'unknown', 'unknown', 'unknown', 'unknown'), # 16 - Unknown Model
    elif x == '10111':
        generateInfoFromDict('unknown', 'unknown', 'unknown', 'unknown', 'unknown'), # 17 - Unknown Model
    elif x == '11000':
        generateInfoFromDict('unknown', 'unknown', 'unknown', 'unknown', 'unknown'), # 18 - Unknown Model
    elif x == '11001':
        generateInfoFromDict('unknown', 'unknown', 'unknown', 'unknown', 'unknown'), # 19 - Unknown Model
    elif x == '11010':
        generateInfoFromDict('unknown', 'unknown', 'unknown', 'unknown', 'unknown'), # 1a - Unknown Model
    elif x == '11011':
        generateInfoFromDict('unknown', 'unknown', 'unknown', 'unknown', 'unknown'), # 1b - Unknown Model
    elif x == '11100':
        generateInfoFromDict('unknown', 'unknown', 'unknown', 'unknown', 'unknown'), # 1c - Unknown Model
    elif x == '11101':
        generateInfoFromDict('unknown', 'unknown', 'unknown', 'unknown', 'unknown'), # 1d - Unknown Model
    elif x == '11110':
        generateInfoFromDict('unknown', 'unknown', 'unknown', 'unknown', 'unknown'), # 1e - Unknown Model
    elif x == '11111':
        generateInfoFromDict('unknown', 'unknown', 'unknown', 'unknown', 'unknown')  # 1f - Unknown Model

def generateInfoFromDict(memory_size, manufacturer, processor, board_type, board_revision):
    generateInfo(memory_sizes[memory_size], manufacturers[manufacturer], processors[processor], board_types[board_type], board_revisions[board_revision])

def generateInfo(memory_size_in, manufacturer_in, processor_in, board_type_in, board_revision_in):
    global memory_size
    global manufacturer
    global processor
    global board_type
    global board_revision
    memory_size = memory_size_in
    manufacturer = manufacturer_in
    processor = processor_in
    board_type = board_type_in
    board_revision = board_revision_in

def toBinary(string):
    processed = BitArray(hex=string.rstrip('\r\n'))
    return processed.bin

def toHex(string):
    processed = BitArray(hex=string.rstrip('\r\n'))
    return processed

def getData(loc):
    region = regions[loc]
    offset = 8
    return data[region - offset]

def getBinary(loc):
    return toBinary(getData(loc))

def getHex(loc):
    return toHex(getData(loc))

def readOTP():
    if len(sys.argv) > 1: # We're given an argument on the command line
        if os.path.isfile(sys.argv[1]):
            with open(sys.argv[1], 'r') as f:
                __readOTPInner(f)
        else:
            print('Unable to open file.')
            sys.exit()
    else: # Use stdin instead.
        __readOTPInner(sys.stdin)
		
def __readOTPInner(myfile):
    for line in myfile:
        unprocessed = line.split(':', 1)[1]
        data.append(unprocessed.rstrip('\r\n'))

readOTP()
processBootMode()
processSerial()
processRevision()

temp = int(unknown_16('bits_0_to_15'), 2)
print ('  OTP Region 16 ( 0-23) :', temp, '(', hex(temp), ')', unknown_16('bits_0_to_15'))
temp = int(unknown_16('bits_16_to_23'), 2)
print ('  OTP Region 16 (24-27) :', temp, '(', hex(temp), ')', unknown_16('bits_16_to_23'))
temp = int(unknown_16('bits_24_to_31'), 2)
print ('  OTP Region 16 (28-31) :', temp, '(', hex(temp), ')', unknown_16('bits_24_to_31'))
print ('               Bootmode :', getHex('bootmode'), getBinary('bootmode'))
print ('        Bootmode - Copy :', getHex('bootmode_copy'))
print ('  OSC Frequency 19.2MHz :', bootmode('bit_1'))
print ('    SDIO Pullup Enabled :', bootmode('bit_3'))
print ('          GPIO Bootmode :', bootmode('bit_19'))
print ('     GPIO Bootmode Bank :', bootmode('bit_20'))
print ('        SD Boot Enabled :', bootmode('bit_21'))
print ('              Boot Bank :', bootmode('bit_22'))
print ('     OTP Region 17 (25) :', bootmode('bit_25'), '(This is Unknown but set on the CM3)')
print ('USB Device Boot Enabled :', bootmode('bit_28'))
print ('  USB Host Boot Enabled :', bootmode('bit_29'))
temp = int(unknown_27('bits_0_to_15'), 2)
print ('  OTP Region 27 ( 0-15) :', temp, '(', hex(temp), ')', unknown_27('bits_0_to_15'))
temp = int(unknown_27('bits_16_to_31'), 2)
print ('  OTP Region 27 (16-31) :', temp, '(', hex(temp), ')', unknown_27('bits_16_to_31'))
print ('          Serial Number :', getHex('serial_number'))
print ('  Inverse Serial Number :', getHex('serial_number_inverted'))
print ('        Revision Number :', getHex('revision_number'))
print ('      New Revision Flag :', revision('new_flag'))
print ('                    RAM :', memory_sizes_asString[memory_size], "MB")
print ('           Manufacturer :', manufacturers_asString[manufacturer])
print ('                    CPU :', processors_asString[processor])
print ('             Board Type : Raspberry Pi Model', board_types_asString[board_type]) # TODO: Check if any of the 4 high bits are set and if they are, handle differently for now.
print ('         Board Revision :', board_revisions_asString[board_revision])
print ('           Batch Number :', getHex('batch_number'))
print ('Overvolt Protection Bit :', overclock('overvolt_protection'))
print ('    Customer Region One :', getHex('customer_one'))
print ('    Customer Region Two :', getHex('customer_two'))
print ('  Customer Region Three :', getHex('customer_three'))
print ('   Customer Region Four :', getHex('customer_four'))
print ('   Customer Region Five :', getHex('customer_five'))
print ('    Customer Region Six :', getHex('customer_six'))
print ('  Customer Region Seven :', getHex('customer_seven'))
print ('  Customer Region Eight :', getHex('customer_eight'))
print ('  Codec License Key One :', getHex('codec_key_one'))
print ('  Codec License Key Two :', getHex('codec_key_two'))
print ('            MAC Address :', formatMAC())
print ('          Advanced Boot :', getHex('advanced_boot'), getBinary('advanced_boot'))
temp = int(advanced_boot('bits_0_to_6'), 2)
print ('     ETH_CLK Output Pin :', temp, '(', hex(temp), ')')
print (' ETH_CLK Output Enabled :', advanced_boot('bit_7'))
temp = int(advanced_boot('bits_8_to_14'), 2)
print ('     LAN_RUN Output Pin :', temp, '(', hex(temp), ')')
print (' LAN_RUN Output Enabled :', advanced_boot('bit_15'))
print ('        USB Hub Timeout :', processABHubTimeout(advanced_boot('bit_24')))
print ('      ETH_CLK Frequency :', processABETH_CLKFrequency(advanced_boot('bit_25')))
