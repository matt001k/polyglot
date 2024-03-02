#!/bin/sh

openssl ecparam -name secp256k1 -genkey -noout -out private.pem
openssl ec -in private.pem -pubout -out public.pem
openssl dgst -sha256 -sign private.pem -out ./test.txt.sign ./test.txt
openssl dgst -sha256 -verify public.pem -signature ./test.txt.sign test.txt
openssl ec -in private.pem -text -out hex_keys.txt
