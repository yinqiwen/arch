#!/bin/sh

shmids=`ipcs -m | grep $USER | awk '{ print $2 }'`
for shmid in $shmids
do
 echo "remove shm:$shmid"
 ipcrm -m $shmid
done

sems=`ipcs -s | grep $USER | awk '{ print $2 }'`
for sem in $sems
do
 echo "remove sem:$sem"
 ipcrm -s $sem
done


mqs=`ipcs -q | grep $USER | awk '{ print $2 }'`
for mq in $mqs
do
 echo "remove mq:$mq"
 ipcrm -q $mq
done

