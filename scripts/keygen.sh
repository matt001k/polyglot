#!/bin/bash

help()
{
    echo "
    Usage: $(basename "${BASH_SOURCE[0]}") [-t type] [-d output] [-e] [-a]

    Create Necessary Keys For Signing And Encrypting Application

    Available Options:

    -h          Print this help message and exit
    -t          Type of ecdh key to generate, valid options are
                    - secp256k1
                    - secp160r1
                    - secp192r1
                    - secp224r1
                    - secp256r1
    -d          Output directory of generated keys
    -e          Withold ECDH key generation
    "
}

OPTIND=1
BASEDIR=$(dirname $0)

# Initialize our own variables:
directory="$BASEDIR/../keys"
type="secp256k1"
ecdh=1
aes=1

while getopts "h?t:d:ea" opt; do
  case "$opt" in
    h|\?)
      help
      exit 0
      ;;
    t)  type=$OPTARG
      ;;
    d)  directory=$OPTARG
      ;;
    e)  ecdh=0
      ;;
    a)  aes=0
      ;;
  esac
done

mkdir -p $directory
if [[ $ecdh -eq 1 ]];
then
    echo "ECDH Key Type: $type"
    openssl ecparam -name $type -genkey -noout -out $directory/ecdh_private.pem
    openssl ec -in $directory/ecdh_private.pem -pubout \
        -out $directory/ecdh_public.pem
    echo "Writing Raw Keys"
    openssl ec -in $directory/ecdh_private.pem -text -noout \
        -out $directory/key_raw.txt
fi

if [[ $aes -eq 1 ]];
then
    echo "Generating AES Key"
    hexdump -vn16 -e'4/4 "%08X" 1 "\n"' /dev/urandom \
        | tr -d '\n' > $directory/aes.txt
    cat $directory/aes.txt | xxd -r -p > $directory/aes.bin
fi
