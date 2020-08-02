#!/bin/bash

bash ./qemu-ifup.sh

qemu-system-arm \
	-M versatilepb \
	-m 128M \
	-nographic \
	-kernel ninjastorms \
	-device e1000,netdev=net0 \
	-netdev tap,id=net0,ifname=tap0,script=no,downscript=no \
	-object filter-dump,id=f1,netdev=net0,file=net_dump.dat > output.out &
TASK_PID=$!
sleep 15
kill $TASK_PID

bash ./qemu-ifdown.sh

echo "Got output"
echo "----------"
cat output.out
echo "----------"
echo "Diffing output"
diff --strip-trailing-cr output.out test/expected_output.out