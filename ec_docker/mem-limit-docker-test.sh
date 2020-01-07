#!/bin/bash

IP_ADDRESS=$1
CONTAINER_NAME=$2
PORT=$3

CONTAINER_ID="$(sudo docker run -d --rm  --cpu-period=100000 --cpu-quota=50000 progrium/stress --vm 1 --vm-bytes 30M)"
CONTAINER_PID="$(sudo docker inspect -f '{{.State.Pid}}' "$CONTAINER_ID")"
echo "container id: "$CONTAINER_ID""
echo "container init pid: "$CONTAINER_PID""

export KERNEL_HOME="../"
../ec_syscalls/sys_connect "$IP_ADDRESS" "$CONTAINER_PID" "$PORT"