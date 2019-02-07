#!/bin/bash

if [[ "$#" != "2" ]]; then
    echo "usage: license.sh <license_generator_path> <license_validator_path>"
    exit 1
fi

#------------------------------------------------------
# Where to find the license programs
#------------------------------------------------------

LICENSE_GENERATOR=$1
LICENSE_VALIDATOR=$2


#------------------------------------------------------
# RSA test keys
#------------------------------------------------------

PRIVATE_KEY="$(git rev-parse --show-toplevel)/apps/license/license_generator/resources/private.pem"


#------------------------------------------------------
# Platform information
#------------------------------------------------------

INTERFACE=$(ls -m /sys/class/net | cut -f 1 -d ",")
MAC_ADDRESS=$(cat /sys/class/net/$INTERFACE/address)


cat > license_test.ini <<EOF
[information]
company = Wavecruncher
expiration = $(date --date='next year' +%Y-%m-%d)
hostname = $HOSTNAME
interface = $INTERFACE
address = $MAC_ADDRESS
EOF

$LICENSE_GENERATOR --private $PRIVATE_KEY license_test.ini || exit 1
VERIPARSE_LICENSE_FILE=license_test.dat $LICENSE_VALIDATOR || exit 1
