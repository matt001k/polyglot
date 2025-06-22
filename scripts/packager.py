import getopt, sys, os, ctypes
from hashlib import sha256
from ecdsa import SigningKey
from ecdsa.util import sigencode_string
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from cryptography.hazmat.backends import default_backend

HELP = 'Usage: ' + os.path.basename(__file__) + \
''' [-h] -i input -o output -a AES -p pem

Encrypt, Sign And Package Firmware To Be Loaded With Secure Boot

Available Options:

-h          Print this help message and exit
-i          Input file to be encrypted, signed and packaged
-a          AES key file (binary representation, see keygen.sh)
-p          ECDH private PEM file
-o          Output directory (relative) of encrypted, signed and packaged file
'''

SIGNATURE_LENGTH=64
IV_LENGTH=16
RESERVED3_LEN=16

class table(ctypes.Structure):
    _pack_ = 1
    _fields_ = [
        ('magic', ctypes.c_uint16),
        ('length', ctypes.c_uint32),
        ('reserved1', ctypes.c_uint32),
        ('crc', ctypes.c_uint32),
        ('reserved2', ctypes.c_uint16),
        ('signature', ctypes.c_uint8 * SIGNATURE_LENGTH),
        ('iv', ctypes.c_uint8 * IV_LENGTH),
        ('reserved3', ctypes.c_uint8 * RESERVED3_LEN),
    ]
    MAGIC = 0xCAFE
    def __init__(self, length=0, signature=0, iv=0):
        self.magic = self.MAGIC
        self.length = length
        self.reserved1 = 0
        self.crc = 0
        self.reserved2 = 0
        if len(signature) != SIGNATURE_LENGTH:
            print('Invalid signature length')
            sys.exit(2)
        for i in range(SIGNATURE_LENGTH):
            self.signature[i] = signature[i]
        if len(iv) != IV_LENGTH:
            print('Invalid IV length')
            sys.exit(2)
        for i in range(IV_LENGTH):
            self.iv[i] = iv[i]
        for i in range(RESERVED3_LEN):
            self.reserved3[i] = 0


class ECDH:
    def sign(self, data, pem):
        key = SigningKey.from_pem(pem)
        signature = key.sign_deterministic(data,
                                           sha256,
                                           sigencode_string)
        return signature

class pad:
    def pad(self, data):
        remainder = 16 - len(data) % 16
        if remainder != 0:
            data += b'\0' * remainder
        return data

class AES:
    def _iv(self):
        return os.urandom(16)
    def encrypt(self, data, key):
        iv = self._iv()
        cipher = Cipher(algorithms.AES(key),
                        modes.CBC(iv),
                        backend=default_backend())
        encryptor = cipher.encryptor()
        ciphertext = encryptor.update(data) + encryptor.finalize()
        return ciphertext, iv
    def decrypt(self, data, key, iv):
        cipher = Cipher(algorithms.AES(key),
                        modes.CBC(iv),
                        backend=default_backend())
        decryptor = cipher.decryptor()
        plaintext = decryptor.update(data) + decryptor.finalize()
        return plaintext


def main():
    try:
        data = 0
        key = 0
        pem = 0
        outfile = 0
        infile = 0
        options = 'hi:a:p:o:'
        long = ['help', 'input', 'aes', 'pem', 'output']
        args, vals = getopt.getopt(sys.argv[1:], options, long)
        for arg, val in args:
            if arg in ('-h', '--help'):
                print(HELP)
                sys.exit(0)
            if arg in ('-i', '--input'):
                infile = os.path.basename(val)
                with open(val, 'rb') as f:
                    data = f.read()
            if arg in ('-a', '--aes'):
                with open(val, 'rb') as f:
                    key = f.read()
            if arg in ('-p', '--pem'):
                with open(val, 'rb') as f:
                    pem = f.read()
            if arg in ('-o', '--output'):
                outfile = val + '/' + infile + '.enc'
    except getopt.error as err:
        print(str(err))
        sys.exit(2)
    # Ensure proper data
    if (data != 0 and key != 0 and pem != 0 and outfile != 0):
        # Pad data to nearest 16 bytes
        data = pad().pad(data)
        # Encrypt data and verify that it was done properly
        ciphertext, iv = AES().encrypt(data, key)
        plaintext = AES().decrypt(ciphertext, key, iv)
        for i in range(min(len(plaintext), len(data))):
            if plaintext[i] != data[i]:
                print('Invalid Value In Encryption Check')
                print('Ensure Proper AES Key')
                sys.exit(1)
        print('File Properly Encrypted')
        # Sign encrypted data
        signature = ECDH().sign(ciphertext, pem)
        print('Encrypted File Properly Signed')
        # Generate table to be sent over and write output file
        with open(outfile, 'wb') as f:
            f.write(table(len(ciphertext), signature, iv))
            f.write(ciphertext)
        print('Output File Written To: ' + outfile)
        sys.exit(0)
    else:
        print('Ensure All Options Specified')
        sys.exit(2)

if __name__ == '__main__':
    main()
