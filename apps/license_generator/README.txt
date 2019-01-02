Build instructions
==================

./configure.sh --release --build


Generating Private/Public RSA keys
==================================

openssl genrsa -out private.pem 2048
openssl rsa -in private.pem -outform PEM -pubout -out public.pem
