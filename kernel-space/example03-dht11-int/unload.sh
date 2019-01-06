#!/bin/sh
module="example03-dht11-int"
device="example03-dht11-int"

rm -f /dev/${device}
rmmod ${module} || true
