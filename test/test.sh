#!/bin/bash

status=0

echo "## Setup"
bash ./qemu-ifup.sh

echo "## Starting OS"
qemu-system-arm \
	-M versatilepb \
	-m 128M \
	-nographic \
	-kernel ninjastorms \
	-device e1000,netdev=net0 \
	-netdev tap,id=net0,ifname=tap0,script=no,downscript=no \
	-object filter-dump,id=f1,netdev=net0,file=net_dump.dat > output.out 2>/dev/null &
TASK_PID=$!

# let the system start
sleep 5
# check that network is working (currently not working on CI)
# echo "## Testing network"
# ip addr show dev br0
# arping -V
# sudo arping -c 5 -I br0 169.254.1.2
# status=$(( $status || $? ))

# enough
kill $TASK_PID

echo "## Checking output"
echo "## ----------"
cat output.out
echo "## ----------"
echo "## Diffing output"
diff --strip-trailing-cr output.out test/expected_output.out
status=$(( $status || $? ))

echo "## Teardown"
bash ./qemu-ifdown.sh
rm output.out
echo "## Testing done!"

exit $status
