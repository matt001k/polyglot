from cryptography.hazmat.primitives import serialization
import sys
import binascii
import os

"""
@brief Convert pem file to header file for ecdh keys

@detail Converts an ecdh pem file to something usable by the header

@param pem_file the file to convert to hex
@param output_dir directory to output header file to
"""
def pem_to_hex(pem_file, output_dir):
    header_file = output_dir + '/ecdh_key.h'
    with open(pem_file, 'rb') as f:
        pem_data = f.read()
    public_key = serialization.load_pem_public_key(pem_data)
    uncompressed_point = public_key.public_bytes(
        encoding=serialization.Encoding.X962,
        format=serialization.PublicFormat.UncompressedPoint
    )
    hex_data = binascii.hexlify(uncompressed_point).decode('ascii')
    hex_formatted_data = ''
    for i in range(0, len(hex_data), 2):
        pair = hex_data[i:i+2]
        hex_formatted_data = hex_formatted_data + '0x' + str(pair) + ', '
    var = 'uint8_t ecdh_key[] = { ' + hex_formatted_data + '};'
    os.makedirs(output_dir, exist_ok=True)
    with open(header_file, 'w') as f:
        f.write('#pragma once\n\n' + var + '\n')
    print('Completed Pem To Header Translation To: ' + output_dir)

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print("Usage: python pem_to_hex.py input_pem_file output_hex_file")
    else:
        pem_file = sys.argv[1]
        output_dir = sys.argv[2]
        pem_to_hex(pem_file, output_dir)
