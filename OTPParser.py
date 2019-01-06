#!/usr/bin/python
"""Raspberry Pi OTP Dump Parser

 Copyright 2019 Jasmine Iwanek & Dylan Morrison

 Permission is hereby granted, free of charge, to any person obtaining a copy of this
 software and associated documentation files (the "Software"), to deal in the Software
 without restriction, including without limitation the rights to use, copy, modify, merge,
 publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons
 to whom the Software is furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED *AS IS*, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
 FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 DEALINGS IN THE SOFTWARE.

 Usage
 call either ./OTPParser.py <filename> or vgcencmd otp_dump | OTPParser
"""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function
from __future__ import unicode_literals
from builtins import int
from builtins import hex
from builtins import open
from builtins import range
import sys
import os.path
try:
    from future import standard_library
    standard_library.install_aliases()
except ImportError:
    print('OTPParser requires future!')
try:
    from bitstring import BitArray
except ImportError:
    sys.exit('OTPParser requires bitstring!')

MEMORY_SIZES = {
    '256':       '000',    # 0
    '512':       '001',    # 1
    '1024':      '010',    # 2
    'unknown_3': '011',    # 3
    'unknown_4': '100',    # 4
    'unknown_5': '101',    # 5
    'unknown_6': '110',    # 6
    'unknown_7': '111',    # 7
    '256/512':   'EITHER',
    'unknown': ''
}
MEMORY_SIZES_AS_STRING = {v: k for k, v in list(MEMORY_SIZES.items())}

MANUFACTURERS = {
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
    'Qisda':      'QISD', # Correct value unknown
    'unknown':    ''
}
MANUFACTURERS_AS_STRING = {v: k for k, v in list(MANUFACTURERS.items())}

PROCESSORS = {
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
PROCESSORS_AS_STRING = {v: k for k, v in list(PROCESSORS.items())}

BOARD_TYPES = {
    'A':         '00000000', # 0
    'B':         '00000001', # 1
    'A+':        '00000010', # 2
    'B+':        '00000011', # 3
    '2B':        '00000100', # 4
    'Alpha':     '00000101', # 5
    'CM1':       '00000110', # 6
    'Unknown_7': '00000111', # 7 (Not in known use)
    '3B':        '00001000', # 8
    'Zero':      '00001001', # 9
    'CM3':       '00001010', # a
    'Unknown_b': '00001011', # b (Not in known use)
    'Zero W':    '00001100', # c
    '3B+':       '00001101', # d
    '3A+':       '00001110', # e
    'unknown_f': '00001111', # f (Not in known use)
    'unknown':   ''
}
BOARD_TYPES_AS_STRING = {v: k for k, v in list(BOARD_TYPES.items())}

BOARD_REVISIONS = {
    '1.0':       '0000',
    '1.1':       '0001',
    '1.2':       '0010',
    '1.3':       '0011',
    'unknown_4': '0100',
    'unknown_5': '0101',
    'unknown_6': '0110',
    'unknown_7': '0111',
    'unknown_8': '1000',
    'unknown_9': '1001',
    'unknown_a': '1010',
    'unknown_b': '1011',
    'unknown_c': '1100',
    'unknown_d': '1101',
    'unknown_e': '1110',
    'unknown_f': '1111',
    '2.0':       ' 2.0', # Correct Value unknown.
    'unknown':   ''
}
BOARD_REVISIONS_AS_STRING = {v: k for k, v in list(BOARD_REVISIONS.items())}

REGIONS = {
    'unknown_8':               8,
    'unknown_9':               9,
    'unknown_10':             10,
    'unknown_11':             11,
    'unknown_12':             12,
    'unknown_13':             13,
    'unknown_14':             14,
    'unknown_15':             15,
    'unknown_16':             16, # Usually 00280000, sometimes 2428000 or 6c28000, as yet unknown
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

MEMORY_SIZE = '000'
MANUFACTURER = '0000'
PROCESSOR = '0000'
BOARD_TYPE = '00000000'
BOARD_REVISION = '0000'

DATA = []

def unknown_16(name):
    """Handler for region 16."""
    indices = {
        'bits_0_to_15':  (16, 32),
        'bits_16_to_23': (8, 16), # only 0x28 seen thus far
        'bits_24_to_31': (0, 8)  # 0x24 & 0x6c seen here thus far
    }[name]
    __unknown_16 = get_binary('unknown_16')
    return __unknown_16[indices[0] : indices[1]]

def bootmode(name):
    """Handler for region 17."""
    indices = {
        'bit_0':         (31, 32), # Unknown (Gordon hinted the Pi wouldn't boot with this set)
        'bit_1':         (30, 31), # Sets the oscillator frequency to 19.2MHz
        'bit_2':         (29, 30), # Unknown (Gordon hinted the Pi wouldn't boot with this set)
        'bit_3':         (28, 29), # Enables pull ups on the SDIO pins
        'bits_4_to_18':  (13, 28), # Unknown/Unused
        'bit_19':        (12, 13), # Enables GPIO bootmode
        'bit_20':        (11, 12), # Sets the bank to check for GPIO bootmode
        'bit_21':        (10, 11), # Enables booting from SD card
        'bit_22':        (9, 10), # Sets the bank to boot from (That's what Gordon said, I think it means SD Card though)
        'bits_26_to_27': (7, 9), # Unknown/Unused
        'bit_25':        (6, 7), # Unknown (Is set on the Compute Module 3)
        'bits_23_to_24': (4, 6), # Unknown/Unused
        'bit_28':        (3, 4), # Enables USB device booting
        'bit_29':        (2, 3), # Enables USB host booting (Ethernet and Mass Storage)
        'bits_30_31':    (0, 2)  # Unknown/Unused
    }[name]
    __bootmode = get_binary('bootmode')
    return __bootmode[indices[0] : indices[1]]

def unknown_27(name):
    """Handler for region 27."""
    indices = {
        'bits_0_to_15':  (16, 32), # 5050 (1B, 2B 1.1), 7373 (2B 1.2), 2727(CM3), 1f1f (3B+)
        'bits_16_to_31': (0, 16)
    }[name]
    __unknown_27 = get_binary('unknown_27')
    return __unknown_27[indices[0] : indices[1]]

def revision(name):
    """Handler for region 30."""
    indices = {
        'legacy_board_revision': (27, 32), # Region used to store the legacy revision, only use if new_flag is not set. Note: it's actually 8 bits, but only 5 were ever used
        'board_revision':        (28, 32), # Revision of the board
        'board_type':            (20, 28), # Model of the board
        'processor':             (16, 20), # Installed Processor
        'manufacturer':          (12, 16), # Manufacturer of the board
        'memory_size':           (9, 12), # Amount of RAM the board has
        'new_flag':              (8, 9), # If set, this board uses the new versioning scheme
        'bits_24_to_31':         (0, 8)  # Unused
    }[name]
    __revision = get_binary('revision_number')
    return __revision[indices[0] : indices[1]]

def overclock(name):
    """Handler for region 32."""
    indices = {
        'overvolt_protection': (31, 32), # Overvolt protection bit
        'bits_0_to_30':        (0, 31)  # Unknown/Unused
    }[name]
    __overclock = get_binary('overclock')
    return __overclock[indices[0] : indices[1]]

def advanced_boot(name):
    """Handler for region 66."""
    indices = {
        'bits_0_to_6':   (25, 32), # GPIO for ETH_CLK output pin
        'bit_7':         (24, 25), # Enable ETH_CLK output pin
        'bits_8_to_14':  (17, 24), # GPIO for LAN_RUN output pin
        'bit_15':        (16, 17), # Enable LAN_RUN output pin
        'bits_16_to_23': (8, 16), # Unknown/Unused
        'bit_24':        (7, 8), # Extend USB HUB timeout parameter
        'bit_25':        (6, 7), # ETH_CLK Frequency (0 = 25MHz, 1 = 24MHz)
        'bits_26_to_31': (0, 6)  # Unknown/Unused
    }[name]
    __advanced_boot = get_binary('advanced_boot')
    return __advanced_boot[indices[0] : indices[1]]

def process_bootmode():
    """Process bootmode, Check against the backup."""
    bootmode_primary = get_binary('bootmode')
    bootmode_copy = get_binary('bootmode_copy')
    if bootmode_primary != bootmode_copy:
        print('Bootmode fields are not the same, this is a bad thing!')

def process_serial():
    """Process Serial, Check against Inverse Serial."""
    serial = get_hex('serial_number')
    inverse_serial = get_hex('serial_number_inverted')
    if (serial ^ inverse_serial) != '0xffffffff':
        print('Serial failed checksum!')

def process_revision():
    """Process Revision, Handle depending on wether it's old or new style."""
    flag = revision('new_flag')
    if flag == '0':
        generate_info_legacy(revision('legacy_board_revision'))
    elif flag == '1':
        generate_info(revision('memory_size'), revision('manufacturer'), revision('processor'), revision('board_type'), revision('board_revision'))

def format_mac():
    """Format MAC Address in a human readalbe fashion."""
    mac_part_1 = get_data('mac_address_one')
    mac_part_2 = get_data('mac_address_two')
    if not mac_part_1 == '00000000':
        mac = mac_part_1 + mac_part_2
        return ':'.join(mac[i:i+2] for i in range(0, 12, 2))
    return 'None'

def process_hub_timeout(bit):
    """Return the HUB timeout."""
    if bit == '1':
        return '5 Seconds'
    return '2 Seconds'

def process_eth_clk_frequency(bit):
    """Return the ETH_CLK frequency."""
    if bit == '1':
        return '24MHz'
    return '25MHz'

def generate_info_legacy(bits):
    """Generate information for legacy board revision."""
    if bits == '00000':   # 00 - Unknown Model
        generate_info_from_dict('unknown', 'unknown', 'unknown', 'unknown', 'unknown')
    elif bits == '00001': # 01 - Unknown Model
        generate_info_from_dict('unknown', 'unknown', 'unknown', 'unknown', 'unknown')
    elif bits == '00010': # 02
        generate_info_from_dict('256', 'Egoman', 'BCM2835', 'B', '1.0')
    elif bits == '00011': # 03
        generate_info_from_dict('256', 'Egoman', 'BCM2835', 'B', '1.0')
    elif bits == '00100': # 04
        generate_info_from_dict('256', 'Sony UK', 'BCM2835', 'B', '2.0')
    elif bits == '00101': # 05
        generate_info_from_dict('256', 'Qisda', 'BCM2835', 'B', '2.0')
    elif bits == '00110': # 06
        generate_info_from_dict('256', 'Egoman', 'BCM2835', 'B', '2.0')
    elif bits == '00111': # 07
        generate_info_from_dict('256', 'Egoman', 'BCM2835', 'A', '2.0')
    elif bits == '01000': # 08
        generate_info_from_dict('256', 'Sony UK', 'BCM2835', 'A', '2.0')
    elif bits == '01001': # 09
        generate_info_from_dict('256', 'Qisda', 'BCM2835', 'A', '2.0')
    elif bits == '01010': # 0a - Unknown Model
        generate_info_from_dict('unknown', 'unknown', 'unknown', 'unknown', 'unknown')
    elif bits == '01011': # 0b - Unknown Model
        generate_info_from_dict('unknown', 'unknown', 'unknown', 'unknown', 'unknown')
    elif bits == '01100': # 0c - Unknown Model
        generate_info_from_dict('unknown', 'unknown', 'unknown', 'unknown', 'unknown')
    elif bits == '01101': # od
        generate_info_from_dict('512', 'Egoman', 'BCM2835', 'B', '2.0')
    elif bits == '01110': # 0e
        generate_info_from_dict('512', 'Sony UK', 'BCM2835', 'B', '2.0')
    elif bits == '01111': # 0f
        generate_info_from_dict('512', 'Egoman', 'BCM2835', 'B', '2.0')
    elif bits == '10000': # 10
        generate_info_from_dict('512', 'Sony UK', 'BCM2835', 'B+', '1.0')
    elif bits == '10001': # 11
        generate_info_from_dict('512', 'Sony UK', 'BCM2835', 'CM1', '1.0')
    elif bits == '10010': #12
        generate_info_from_dict('512', 'Sony UK', 'BCM2835', 'A+', '1.1')
    elif bits == '10011': # 13
        generate_info_from_dict('512', 'Embest', 'BCM2835', 'B+', '1.2')
    elif bits == '10100': # 14
        generate_info_from_dict('512', 'Embest', 'BCM2835', 'CM1', '1.0')
    elif bits == '10101': # 15 - This can be 256MB or 512MB
        generate_info_from_dict('256/512', 'Embest', 'BCM2835', 'A+', '1.1')
    elif bits == '10110': # 16 - Unknown Model
        generate_info_from_dict('unknown', 'unknown', 'unknown', 'unknown', 'unknown')
    elif bits == '10111': # 17 - Unknown Model
        generate_info_from_dict('unknown', 'unknown', 'unknown', 'unknown', 'unknown')
    elif bits == '11000': # 18 - Unknown Model
        generate_info_from_dict('unknown', 'unknown', 'unknown', 'unknown', 'unknown')
    elif bits == '11001': # 19 - Unknown Model
        generate_info_from_dict('unknown', 'unknown', 'unknown', 'unknown', 'unknown')
    elif bits == '11010': # 1a - Unknown Model
        generate_info_from_dict('unknown', 'unknown', 'unknown', 'unknown', 'unknown')
    elif bits == '11011': # 1b - Unknown Model
        generate_info_from_dict('unknown', 'unknown', 'unknown', 'unknown', 'unknown')
    elif bits == '11100': # 1c - Unknown Model
        generate_info_from_dict('unknown', 'unknown', 'unknown', 'unknown', 'unknown')
    elif bits == '11101': # 1d - Unknown Model
        generate_info_from_dict('unknown', 'unknown', 'unknown', 'unknown', 'unknown')
    elif bits == '11110': # 1e - Unknown Model
        generate_info_from_dict('unknown', 'unknown', 'unknown', 'unknown', 'unknown')
    elif bits == '11111': # 1f - Unknown Model
        generate_info_from_dict('unknown', 'unknown', 'unknown', 'unknown', 'unknown')

def generate_info_from_dict(memory_size_in, manufacturer_in, processor_in, board_type_in, board_revision_in):
    """Generate information for legacy Board revision (Prettyness wrapper)."""
    generate_info(MEMORY_SIZES[memory_size_in], MANUFACTURERS[manufacturer_in], PROCESSORS[processor_in], BOARD_TYPES[board_type_in], BOARD_REVISIONS[board_revision_in])

def generate_info(memory_size_in, manufacturer_in, processor_in, board_type_in, board_revision_in):
    """Generate information for board revision."""
    global MEMORY_SIZE
    global MANUFACTURER
    global PROCESSOR
    global BOARD_TYPE
    global BOARD_REVISION
    MEMORY_SIZE = memory_size_in
    MANUFACTURER = manufacturer_in
    PROCESSOR = processor_in
    BOARD_TYPE = board_type_in
    BOARD_REVISION = board_revision_in

def to_binary(string):
    """Convert string to binary."""
    processed = BitArray(hex=string.rstrip('\r\n'))
    return processed.bin

def to_hex(string):
    """Convert string to hexidecimal"""
    processed = BitArray(hex=string.rstrip('\r\n'))
    return processed

def get_data(loc):
    """Get unformatted data from specified OTP region."""
    region = REGIONS[loc]
    offset = 8
    return DATA[region - offset]

def get_binary(loc):
    """Get binary data from specified OTP region."""
    return to_binary(get_data(loc))

def get_hex(loc):
    """Get hexidecimal data from specified OTP region."""
    return to_hex(get_data(loc))

def pretty_string_no_binary(value):
    """Return a pretty OTP etntry (Without binary)."""
    intval = int(value, 2)
    return '' + str(intval) + ' (' + hex(intval) + ') '

def pretty_string(value):
    """Return a pretty OTP entry."""
    intval = int(value, 2)
    return '' + str(intval) + ' (' + hex(intval) + ') ' + value

def read_otp():
    """Read OTP from specified file."""
    if len(sys.argv) > 1: # We're given an argument on the command line
        if os.path.isfile(sys.argv[1]):
            with open(sys.argv[1], 'r') as file:
                __read_otp_inner(file)
        else:
            print('Unable to open file.')
            sys.exit()
    else: # Use stdin instead.
        __read_otp_inner(sys.stdin)

def __read_otp_inner(myfile):
    """Inner part of OTP file reader."""
    for line in myfile:
        try:
            unprocessed = line.split(':', 1)[1]
            DATA.append(unprocessed.rstrip('\r\n'))
        except IndexError:
            sys.exit('Invalid OTP Dump')

read_otp()
process_bootmode()
process_serial()
process_revision()

print('  OTP Region 16 ( 0-23) :', pretty_string(unknown_16('bits_0_to_15')))
print('  OTP Region 16 (24-27) :', pretty_string(unknown_16('bits_16_to_23')))
print('  OTP Region 16 (28-31) :', pretty_string(unknown_16('bits_24_to_31')))
print('               Bootmode :', get_hex('bootmode'), get_binary('bootmode'))
print('        Bootmode - Copy :', get_hex('bootmode_copy'))
print('  OSC Frequency 19.2MHz :', bootmode('bit_1'))
print('    SDIO Pullup Enabled :', bootmode('bit_3'))
print('          GPIO Bootmode :', bootmode('bit_19'))
print('     GPIO Bootmode Bank :', bootmode('bit_20'))
print('        SD Boot Enabled :', bootmode('bit_21'))
print('              Boot Bank :', bootmode('bit_22'))
print('     OTP Region 17 (25) :', bootmode('bit_25'), '(This is Unknown but set on the CM3)')
print('USB Device Boot Enabled :', bootmode('bit_28'))
print('  USB Host Boot Enabled :', bootmode('bit_29'))
print('  OTP Region 27 ( 0-15) :', pretty_string(unknown_27('bits_0_to_15')))
print('  OTP Region 27 (16-31) :', pretty_string(unknown_27('bits_16_to_31')))
print('          Serial Number :', get_hex('serial_number'))
print('  Inverse Serial Number :', get_hex('serial_number_inverted'))
print('        Revision Number :', get_hex('revision_number'))
print('      New Revision Flag :', revision('new_flag'))
print('                    RAM :', MEMORY_SIZES_AS_STRING[MEMORY_SIZE], "MB")
print('           Manufacturer :', MANUFACTURERS_AS_STRING[MANUFACTURER])
print('                    CPU :', PROCESSORS_AS_STRING[PROCESSOR])
try:
    TEMP = 'Raspberry Pi Model' + BOARD_TYPES_AS_STRING[BOARD_TYPE]
except KeyError:
    TEMP = 'unknown_' + hex(int(BOARD_TYPE, 2)).lstrip('0x')
print('             Board Type :', TEMP)
print('         Board Revision :', BOARD_REVISIONS_AS_STRING[BOARD_REVISION])
print('           Batch Number :', get_hex('batch_number'))
print('Overvolt Protection Bit :', overclock('overvolt_protection'))
print('    Customer Region One :', get_hex('customer_one'))
print('    Customer Region Two :', get_hex('customer_two'))
print('  Customer Region Three :', get_hex('customer_three'))
print('   Customer Region Four :', get_hex('customer_four'))
print('   Customer Region Five :', get_hex('customer_five'))
print('    Customer Region Six :', get_hex('customer_six'))
print('  Customer Region Seven :', get_hex('customer_seven'))
print('  Customer Region Eight :', get_hex('customer_eight'))
print('  Codec License Key One :', get_hex('codec_key_one'))
print('  Codec License Key Two :', get_hex('codec_key_two'))
print('            MAC Address :', format_mac())
print('          Advanced Boot :', get_hex('advanced_boot'), get_binary('advanced_boot'))
print('     ETH_CLK Output Pin :', pretty_string_no_binary(advanced_boot('bits_0_to_6')))
print(' ETH_CLK Output Enabled :', advanced_boot('bit_7'))
print('     LAN_RUN Output Pin :', pretty_string_no_binary(advanced_boot('bits_8_to_14')))
print(' LAN_RUN Output Enabled :', advanced_boot('bit_15'))
print('        USB Hub Timeout :', process_hub_timeout(advanced_boot('bit_24')))
print('      ETH_CLK Frequency :', process_eth_clk_frequency(advanced_boot('bit_25')))
