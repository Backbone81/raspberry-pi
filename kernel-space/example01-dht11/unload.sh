#!/bin/sh
module="example01-dht11"
device="example01-dht11"

rm -f /dev/${device}
rmmod ${module} || true
