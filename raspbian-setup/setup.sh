#!/bin/bash
set -e

# download raspbian image
if [ ! -f raspbian-stretch.zip ]; then
	wget -O raspbian-stretch.zip  https://downloads.raspberrypi.org/raspbian/images/raspbian-2018-11-15/2018-11-13-raspbian-stretch.zip
fi
echo "a121652937ccde1c2583fe77d1caec407f2cd248327df2901e4716649ac9bc97 *raspbian-stretch.zip" | sha256sum --check -

# disable automount and unmount any sd card partitions
gsettings set org.gnome.desktop.media-handling automount false
sudo umount /dev/sdb? || true

# copy image on target sd card
unzip -p raspbian-stretch.zip | sudo dd bs=4M of=/dev/sdb status=progress conv=fsync

# mount sd card partitions
if [ ! -d boot ]; then
	mkdir boot
fi
sudo mount /dev/sdb1 boot

# preconfigure wlan
cat << EOF | sudo tee boot/wpa_supplicant.conf
ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
update_config=1
country=DE

network={
	ssid="c8fa9e7acab5c951"
	psk="14e27c7ef27c4d5b"
	key_mgmt=WPA-PSK
}
EOF

# activate ssh
sudo touch boot/ssh

# reenable automount
sudo umount /dev/sdb?
gsettings set org.gnome.desktop.media-handling automount true
