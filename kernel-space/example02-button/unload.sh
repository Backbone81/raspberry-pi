#!/bin/sh
module="example02-button"
device="example02-button"

rm -f /dev/${device}
rmmod ${module} || true
