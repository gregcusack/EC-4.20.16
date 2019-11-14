#!/bin/bash

CONTAINER_NAME=$1
PORT=$2
QUOTA=$(($3 * 1000))
echo $QUOTA

CONTAINER_ID="$(sudo docker run -d --cpu-period=100000 --cpu-quota=$QUOTA --cpuset-cpus 0 "$CONTAINER_NAME")"
CONTAINER_PID="$(sudo docker inspect -f '{{.State.Pid}}' "$CONTAINER_ID")"
echo "container id: "$CONTAINER_ID""
echo "container init pid: "$CONTAINER_PID""

export KERNEL_HOME="../"
../ec_syscalls/sys_connect "$CONTAINER_PID" "$PORT" "$EC_ID"

export CONTAINER_ID
