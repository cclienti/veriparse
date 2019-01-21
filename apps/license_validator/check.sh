#!/bin/bash

if [[ "$#" != "3" ]]; then
    echo "usage: check.sh <license_generator_path> <license_validator_path> <pivate_key>"
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

PRIVATE_KEY=$3  # $(git rev-parse --show-toplevel)/apps/license_generator/resources/private.pem


#------------------------------------------------------
# Platform information
#------------------------------------------------------

INTERFACE=$(ls -m /sys/class/net | cut -f 1 -d ",")
MAC_ADDRESS=$(cat /sys/class/net/$INTERFACE/address)


#------------------------------------------------------
echo "Check next year validity"
#------------------------------------------------------

cat > license_test.ini <<EOF
[information]
company = Wavecruncher
expiration = $(date --date='next year' +%Y-%m-%d)
hostname = $HOSTNAME
interface = $INTERFACE
address = $MAC_ADDRESS
EOF

$LICENSE_GENERATOR --private $PRIVATE_KEY license_test.ini || exit 1
$LICENSE_VALIDATOR license_test.dat || exit 1


#------------------------------------------------------
echo "Check 0-day expiration"
#------------------------------------------------------

cat > license_test.ini <<EOF
[information]
company = Wavecruncher
expiration = $(date +%Y-%m-%d)
hostname = $HOSTNAME
interface = $INTERFACE
address = $MAC_ADDRESS
EOF

$LICENSE_GENERATOR --private $PRIVATE_KEY license_test.ini || exit 1
$LICENSE_VALIDATOR license_test.dat || exit 1


#------------------------------------------------------
echo "Check 31 days expiration"
#------------------------------------------------------

cat > license_test.ini <<EOF
[information]
company = Wavecruncher
expiration = $(date --date='31 days' +%Y-%m-%d)
hostname = $HOSTNAME
interface = $INTERFACE
address = $MAC_ADDRESS
EOF

$LICENSE_GENERATOR --private $PRIVATE_KEY license_test.ini || exit 1
$LICENSE_VALIDATOR license_test.dat || exit 1


#------------------------------------------------------
echo "Check expired validity"
#------------------------------------------------------

cat > license_test.ini <<EOF
[information]
company = Wavecruncher
expiration = $(date --date='yesterday' +%Y-%m-%d)
hostname = $HOSTNAME
interface = $INTERFACE
address = $MAC_ADDRESS
EOF

$LICENSE_GENERATOR --private $PRIVATE_KEY license_test.ini || exit 1
$LICENSE_VALIDATOR license_test.dat && exit 1


#------------------------------------------------------
echo "Check company modification"
#------------------------------------------------------

cat > license_test.ini <<EOF
[information]
company = Wavecruncher
expiration = $(date --date='next year' +%Y-%m-%d)
hostname = $HOSTNAME
interface = $INTERFACE
address = $MAC_ADDRESS
EOF

$LICENSE_GENERATOR --private $PRIVATE_KEY license_test.ini || exit 1
sed -i 's/company = .*$/company = Other/g' license_test.dat
$LICENSE_VALIDATOR license_test.dat && exit 1


#------------------------------------------------------
echo "Check license file expiration modification"
#------------------------------------------------------

cat > license_test.ini <<EOF
[information]
company = Wavecruncher
expiration = $(date --date='next year' +%Y-%m-%d)
hostname = $HOSTNAME
interface = $INTERFACE
address = $MAC_ADDRESS
EOF

$LICENSE_GENERATOR --private $PRIVATE_KEY license_test.ini || exit 1
sed -i 's/expiration = .*$/expiration = 2040-12-12/g' license_test.dat
$LICENSE_VALIDATOR license_test.dat && exit 1


#------------------------------------------------------
echo "Check hostname modification"
#------------------------------------------------------

cat > license_test.ini <<EOF
[information]
company = Wavecruncher
expiration = $(date --date='next year' +%Y-%m-%d)
hostname = $HOSTNAME
interface = $INTERFACE
address = $MAC_ADDRESS
EOF

$LICENSE_GENERATOR --private $PRIVATE_KEY license_test.ini || exit 1
sed -i 's/hostname = .*$/hostname = Other/g' license_test.dat
$LICENSE_VALIDATOR license_test.dat && exit 1


#------------------------------------------------------
echo "Check interface modification"
#------------------------------------------------------

cat > license_test.ini <<EOF
[information]
company = Wavecruncher
expiration = $(date --date='next year' +%Y-%m-%d)
hostname = $HOSTNAME
interface = $INTERFACE
address = $MAC_ADDRESS
EOF

$LICENSE_GENERATOR --private $PRIVATE_KEY license_test.ini || exit 1
sed -i 's/interface = .*$/interface = other42/g' license_test.dat
$LICENSE_VALIDATOR license_test.dat && exit 1


#------------------------------------------------------
echo "Check mac address modification"
#------------------------------------------------------

cat > license_test.ini <<EOF
[information]
company = Wavecruncher
expiration = $(date --date='next year' +%Y-%m-%d)
hostname = $HOSTNAME
interface = $INTERFACE
address = $MAC_ADDRESS
EOF

$LICENSE_GENERATOR --private $PRIVATE_KEY license_test.ini || exit 1
sed -i 's/address = .*$/address = 00:11:22:33:44:55/g' license_test.dat
$LICENSE_VALIDATOR license_test.dat && exit 1


#------------------------------------------------------
echo "Check for other network card"
#------------------------------------------------------

cat > license_test.ini <<EOF
[information]
company = Wavecruncher
expiration = $(date --date='next year' +%Y-%m-%d)
hostname = $HOSTNAME
interface = $INTERFACE
address = 00:11:22:33:44:55
EOF

$LICENSE_GENERATOR --private $PRIVATE_KEY license_test.ini || exit 1
$LICENSE_VALIDATOR license_test.dat && exit 1


#------------------------------------------------------
# End
#------------------------------------------------------

echo "Tests Ok"
exit 0
