from __future__ import print_function
from bitstring import BitArray
import sys
import os.path

memory_sizes = {
    '256':       '000', # 0
    '512':       '001', # 1
    '1024':      '010', # 2
    'unknown_3': '011',
    'unknown_4': '100',
    'unknown_5': '101',
    'unknown_6': '110',
    'unknown_7': '111',
    'unknown': ''
}
memory_sizes_asString = {v: k for k, v in memory_sizes.items()}

manafacturers = {
    'Sony UK':    '0000', # 0
    'Egoman':     '0001', # 1
    'Embest':     '0010', # 2
    'Sony Japan': '0011', # 3
    'Embest #2':  '0100', # 4
    'Stadium':    '0101', # 5
    'unknown_6':  '0110', # 6
    'unknown_7':  '0111', # 7
    'unknown_8':  '1000', # 8
    'unknown_9':  '1001', # 9
    'unknown_a':  '1010', # a
    'unknown_b':  '1011', # b
    'unknown_c':  '1100', # c
    'unknown_d':  '1101', # d
    'unknown_e':  '1110', # e
    'unknown_f':  '1111', # f
    'Qisda':      'QISD', # TODO: Correct value unknown
    'unknown':    ''
}
manafacturers_asString = {v: k for k, v in manafacturers.items()}

processors = {
    'BCM2835':   '0000', # 0
    'BCM2836':   '0001', # 1
    'BCM2837':   '0010', # 2
    'unknown_3': '0011', # 3
    'unknown_4': '0100', # 4
    'unknown_5': '0101', # 5
    'unknown_6': '0110', # 6
    'unknown_7': '0111', # 7
    'unknown_8': '1000', # 8
    'unknown_9': '1001', # 9
    'unknown_a': '1010', # a
    'unknown_b': '1011', # b
    'unknown_c': '1100', # c
    'unknown_d': '1101', # d
    'unknown_e': '1110', # e
    'unknown_f': '1111', # f
    'unknown':   ''

}
processors_asString = {v: k for k, v in processors.items()}

# TODO : This can cause crashes
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
    'unknown_f':  '00001111', # f (Not in known use)
    'unknown_ff': '11111111', # ff (Not in known use)
    'unknown':   ''
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
    'unknown_16': 16,
    'bootmode': 17,
    'bootmode_copy': 18,
    'unknown_27': 27,
    'serial_number': 28,
    'serial_number_inverted': 29,
    'revision_number': 30,
    'batch_number': 31,
    'overclock': 32,
    'customer_one': 36,
    'customer_two': 37,
    'customer_three': 38,
    'customer_four': 39,
    'customer_five': 40,
    'customer_six': 41,
    'customer_seven': 42,
    'customer_eight': 43,
    'codec_key_one': 45,
    'codec_key_two': 46,
    'mac_address_one': 64,
    'mac_address_two': 65,
    'advanced_boot': 66
}

memory_size = '000'
manafacturer = '0000'
processor = '0000'
board_type = '00000000'
board_revision = '0000'

data = []

def bootmode(name):
    indices = {
        'bit_0': (31, 32), # Unknown/Unused
        'bit_1': (30, 31), # Sets the oscilator frequency to 19.2MHz
        'bit_2': (29, 30), # Unknown/Unused
        'bit_3': (28, 29), # Enables pull ups on the SDIO pins
        'bits_4_to_18': (13, 28), # Unknown/Unused
        'bit_19': (12, 13), # Enables GPIO bootmode
        'bit_20': (11, 12), # Sets the bank to check for GPIO bootmode
        'bit_21': (10, 11), # Enables booting from SD card
        'bit_22': (9, 10), # Sets the bank to boot from, That's what Gordon said, I think it means SD Card though.
        'bits_26_to_27': (7, 9), # Unknown/Unused
        'bit_25': (6, 7), # Unknown
        'bits_23_to_24': (4, 6), # Unknown/Unused
        'bit_28': (3, 4), # Enables USB device booting
        'bit_29': (2, 3), # Enables USB host booting (Ethernet and Mass Storage)
        'bits_30_31': (0, 2) # Unknown/Unused
    }[name]
    bootmode = getBinary('bootmode')
    return bootmode[indices[0] : indices[1]]

def revision(name):
    indices = {
        'new_flag': (8, 9), # If set, this board uses the new versioning scheme
        'memory_size': (9, 12), # Amount of RAM the board has
        'manafacturer': (12, 16), # Manafacturer of the board
        'processor': (16, 20), # Installed Processor
        'board_type': (20, 28), #Model of the board
        'board_revision': (28, 32), # Revision of the board
        'legacy_board_revision': (27, 32) # Region used to store the legacy revision, only use if new_flag is not set. Note: it's actually 8 bits, but only 5 were ever used
    }[name]
    revision = getBinary('revision_number')
    return revision[indices[0] : indices[1]]

def overclock(name):
    indices = {
        'overvolt_protection': (31, 32), # Overvolt protection bit
        'bits_1_to_31': (0, 31) #Unknown/Unused
    }[name]
    overclock = getBinary('overclock')
    return overclock[indices[0] : indices[1]]

def advanced_boot(name):
    indices = {
        'bits_0_to_6': (25, 32), # GPIO for ETH_CLK output pin
        'bit_7': (24, 25), # Enable ETH_CLK output pin
        'bits_8_to_14': (17, 24), # GPIO for LAN_RUN output pin
        'bit_15': (16, 17), # Enable LAN_RUN output pin
        'bits_16_to_23': (8, 16), # Unknown/Unused
        'bit_24': (7, 8), # Extend USB Hub timeout parameter
        'bit_25': (6, 7), # ETH_CLK Frequency (0 = 25MHz, 1 = 24MHz)
        'bits_26_to_31': (0, 6) # Unknown/Unused
    }[name]
    advanced_boot = getBinary('advanced_boot')
    return advanced_boot[indices[0] : indices[1]]

def processBootMode():
    bootmode_primary = getBinary('bootmode')
    bootmode_copy = getBinary('bootmode_copy')
    if bootmode_primary != bootmode_copy:
        print('bootmode fields are not the same, this is a bad thing')

def processSerial():
    serial = getBinary('serial_number')
    inverse_serial = getBinary('serial_number_inverted')
    #return inverse_serial == ~serial

def processRevision():
    flag = revision('new_flag')
    if flag == '0':
        print('Old Board Revision')
        generateInfoLegacy(revision('legacy_board_revision'))
    elif flag == '1':
        generateInfo(revision('memory_size'), revision('manafacturer'), revision('processor'), revision('board_type'), revision('board_revision'))

def processMAC():
	# TODO: Verify this is what you want! Will return true or false depending on if the MACs are identical
    return getData('mac_address_one') == getData('mac_address_two')

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
        generateInfoFromDict('512',     'Embest',  'BCM2835', 'A+',      '1.1'    ), # 15 - TODO: This can actually be 256 or 512.
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

def generateInfoFromDict(memory_size, manafacturer, processor, board_type, board_revision):
    generateInfo(memory_sizes[memory_size], manafacturers[manafacturer], processors[processor], board_types[board_type], board_revisions[board_revision])

def generateInfo(memory_size_in, manafacturer_in, processor_in, board_type_in, board_revision_in):
    global memory_size
    global manafacturer
    global processor
    global board_type
    global board_revision
    memory_size = memory_size_in
    manafacturer = manafacturer_in
    processor = processor_in
    board_type = board_type_in
    board_revision = board_revision_in

def toBinary(string):
    processed = BitArray(hex=string.rstrip('\r\n'))
    return processed.bin

def getData(loc):
    region = regions[loc]
    offset = 8
    return data[region - offset]

def getBinary(loc):
    return toBinary(getData(loc))

def readOTP():
	if len(sys.argv) > 1: # We're given an argument on the command line
		if os.path.isfile(sys.argv[1]):
			with open(sys.argv[1], 'r') as f:
				__readOTPinner(f)
		else:
			print('Unable to open file.')
			sys.exit()
	else: # Use stdin instead.
		__readOTPinner(sys.stdin)
		
def __readOTPinner(myfile):
	for line in myfile:
		unprocessed = line.split(':', 1)[1]
		data.append(unprocessed.rstrip('\r\n'))

readOTP()
processBootMode()
processSerial() # This isn't really doing anything yet! You'll want to actually check the return value here.
# EXAMPLE CODE
# if processSerial():
#     print("Serial matches!")
processRevision()
processMAC() # Ditto with processSerial
# EXAMPLE CODE
# if processMAC():
#     print("MACs match!")

print ('OTP Region 16 (Unknown) :', getData('unknown_16'), getBinary('unknown_16'))
print ('               Bootmode :', getData('bootmode'), getBinary('bootmode'))
print ('        Bootmode - Copy :', getData('bootmode_copy'))
print ('  OSC Frequency 19.2MHz :', bootmode('bit_1'))
print ('    SDIO Pullup Enabled :', bootmode('bit_3'))
print ('          GPIO Bootmode :', bootmode('bit_19'))
print ('     GPIO Bootmode Bank :', bootmode('bit_20'))
print ('        SD Boot Enabled :', bootmode('bit_21'))
print ('              Boot Bank :', bootmode('bit_22'))
print ('                  17-25 :', bootmode('bit_25'), '(This is Unknown but set on the CM3 & CM3(NEC))')
print ('USB Device Boot Enabled :', bootmode('bit_28'))
print ('  USB Host Boot Enabled :', bootmode('bit_29'))
print ('OTP Region 27 (Unknown) :', getData('unknown_27'), getBinary('unknown_27'))
print ('          Serial Number :', getData('serial_number'))
print ('  Inverse Serial Number :', getData('serial_number_inverted'))
print ('        Revision Number :', getData('revision_number'))
print ('                    RAM :', memory_sizes_asString[memory_size], "MB")
print ('           Manafacturer :', manafacturers_asString[manafacturer])
print ('                    CPU :', processors_asString[processor])
print ('             Board Type : Raspberry Pi Model', board_types_asString[board_type]) # TODO: Check if any of the 4 high bits are set and if they are, handle differently for now
print ('               Revision :', board_revisions_asString[board_revision])
print ('           Batch Number :', getData('batch_number'))
print ('Overvolt Protection Bit :', overclock('overvolt_protection'))
print ('    Customer Region One :', getData('customer_one'))
print ('    Customer Region Two :', getData('customer_two'))
print ('  Customer Region Three :', getData('customer_three'))
print ('   Customer Region Four :', getData('customer_four'))
print ('   Customer Region Five :', getData('customer_five'))
print ('    Customer Region Six :', getData('customer_six'))
print ('  Customer Region Seven :', getData('customer_seven'))
print ('  Customer Region Eight :', getData('customer_eight'))
print ('  Codec License Key One :', getData('codec_key_one'))
print ('  Codec License Key Two :', getData('codec_key_two'))
print ('      First Mac Address :', getData('mac_address_one')) # Format as MAC
print ('     Second Mac Address :', getData('mac_address_two')) # Format as MAC
print ('          Advanced Boot :', getData('advanced_boot'), getBinary('advanced_boot'))
temp = int(advanced_boot('bits_0_to_6'), 2)
print ('     ETH_CLK Output Pin :', temp, '(', hex(temp), ')')
print (' ETH_CLK Output Enabled :', advanced_boot('bit_7'))
temp = int(advanced_boot('bits_8_to_14'), 2)
print ('     LAN_RUN Output Pin :', temp, '(', hex(temp), ')')
print (' LAN_RUN Output Enabled :', advanced_boot('bit_15'))
print ('        USB Hub Timeout :', advanced_boot('bit_24'), '(0 = 2 Seconds, 1 = 5 Seconds)')
print ('      ETH_CLK Frequency :', advanced_boot('bit_25'), '(0 = 25MHz, 1 = 24MHz)')
