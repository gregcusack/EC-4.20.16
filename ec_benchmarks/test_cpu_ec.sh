#!/bin/bash

CONTAINER_NAME=$1
IP_ADDRESS=$2
quota_array=( 200000 )
#quota_array=( 10000 20000 30000 40000 50000 60000 70000 80000 90000 100000 200000 300000 400000 500000 600000 700000 800000 900000 1000000 )
export KERNEL_HOME="../"
make_dir="$(mkdir "$IP_ADDRESS")"
echo "Made directory to store results"
for i in "${quota_array[@]}"
do
	CONTAINER_ID="$(sudo docker run -d --cpu-period=100000 --cpu-quota="$i" --cpuset-cpus=0-9 "$CONTAINER_NAME")"
	CONTAINER_PID="$(sudo docker inspect -f '{{.State.Pid}}' "$CONTAINER_ID")"
	echo "Running test for period=100000 and quota="$i" with id: "$CONTAINER_ID" and pid: "$CONTAINER_PID""
	EC_RESULT="$(../ec_syscalls/sys_connect "$IP_ADDRESS" "$CONTAINER_PID" 4444)"
	echo "EC syscall result: "$EC_RESULT""
	while [ "`sudo docker inspect -f '{{.State.Running}}' "$CONTAINER_ID"`" != "false" ]
        do
                #echo "Container status: `sudo docker inspect -f '{{.State.Running}}' "$CONTAINER_ID"`"
                sleep 1
        done
	echo "container exited"
	FILE_COPY="$(sudo docker cp "$CONTAINER_ID":/tmp/test.txt "$IP_ADDRESS"/"$i".txt)"
	echo "$(sudo docker rm "$CONTAINER_ID")"
done

