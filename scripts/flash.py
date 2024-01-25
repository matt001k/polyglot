#! /usr/bin/env python

import os
import subprocess
import sys
import tempfile
import shutil
import crc

try:
    from argparse import ArgumentParser as Parser
except ImportError:
    from optparse import OptionParser as Parser

CRC_POLYNOMIAL=0x1EDC6F41
CRC_WIDTH=32
CRC_INITIAL_VALUE=0x00
CRC_REVERSE=True

BL_SECRET_WORD=0x1BADCAFE

DEFAULT_DEVICE="EFM32TG11B520F128"
DEFAULT_ADDRESS="0x8000"

parser = Parser(description='flash binary using a Segger JLink')
parser.add_argument('-s', '--speed', dest='speed', action='store', default=4000,
        help='set the jtag clock speed to operate at (kHz)')
parser.add_argument('-d', '--device', dest='device', action='store', default=DEFAULT_DEVICE,
        help='set the device type being talked to')
parser.add_argument('binary', metavar='BINARY', type=str,
        help='file with binary data to flash via the JLink')
parser.add_argument('address', metavar='ADDRESS', type=str, default=DEFAULT_ADDRESS, nargs='?',
        help='address to flash the binary to')


SCRIPT = """
device {device}
speed {speed}
if SWD
autoconnect 1
loadbin {binary}, {address}
qc
"""

def compute_crc(binary_load_file):
    config = crc.Configuration(CRC_WIDTH,
                               CRC_POLYNOMIAL,
                               CRC_INITIAL_VALUE,
                               0x00,
                               CRC_REVERSE,
                               CRC_REVERSE)
    c = crc.CrcCalculator(config)
    fd = open(binary_load_file, "rb")
    buffer = fd.read()
    fd.close()
    checksum = c.calculate_checksum(buffer)
    return checksum

def create_compatible_file(args):
    checksum = compute_crc(args['binary'])
    print(checksum)
    name = os.path.splitext(args['binary'])[0] + '.crc.bin'
    print(name)
    shutil.copy(args['binary'], name)
    with open(name, 'ab') as fd:
        fd.write(checksum.to_bytes(4, 'big'))
        print(checksum.to_bytes(4, 'big'))
        fd.write(BL_SECRET_WORD.to_bytes(4, 'big'))
        print(BL_SECRET_WORD.to_bytes(4, 'big'))
    return name

def jlink_script(fname):
    return subprocess.call(['JLinkExe', '-CommanderScript', fname])


if __name__ == '__main__':
    args = parser.parse_args()
    args = vars(args)

    # Create a temporary file for the JLink commander script.
    f = tempfile.NamedTemporaryFile(mode='w+', dir='.', delete=False)
    binary_load_file = create_compatible_file(args)
    args['binary'] = binary_load_file

    f.write(SCRIPT.format(**args))
    print(f.read())
    f.close()

    jlink_script(f.name)

    # Delete the commander script.
    os.unlink(f.name)
    os.unlink(binary_load_file)
