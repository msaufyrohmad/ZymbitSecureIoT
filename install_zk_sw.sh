#!/bin/bash

mod=""

# ensure running as root or exit
if [ "$(id -u)" != "0" ]; then
  echo "run this as root or use sudo" 2>&1 && exit 1;
fi;

# insure that the group 'gpio' exists
grep "^gpio" /etc/group &>/dev/null
if [ $? -ne 0 ]
then
  echo "Group 'gpio' does not exist. This group is necessary for zymbit software to operate normally."
  read -p 'Type yes in all capital letters (YES) to create this group: ' answer <&1
  if [ "${answer}" == "YES" ]
  then
    # Add group 'gpio'
    groupadd gpio
    # Modify /etc/rc.local to change the group of /etc/sys/class/gpio
    grep "chown -R root:gpio" /etc/rc.local &>/dev/null
    if [ $? -ne 0 ]
    then
      echo "chown -R root:gpio /sys/class/gpio" >> /etc/rc.local
      echo "chmod -R ug+rw /sys/class/gpio" >> /etc/rc.local
    fi
    # Check for existence of udev rule
    if [ ! -f "/etc/udev/rules.d/80-gpio-noroot.rules" ]
    then
      echo "ACTION==\"add\", SUBSYSTEM==\"gpio\", PROGRAM=\"/bin/sh -c 'chown -R root:gpio /sys/${DEVPATH}; chmod -R g+w /sys/${DEVPATH}'\"" >> /etc/udev/rules.d/80-gpio-noroot.rules
    fi
  else
    echo "Quitting..."
    exit -1
  fi
fi

# for older versions of Raspbian, insure that apt-transport-https is installed first
echo -n "Installing prerequisites..."
apt update --allow-releaseinfo-change -y
apt install -y libboost-thread-dev lsb-release libjansson4 &>/dev/null
apt install -y apt-transport-https libyaml-dev libssl-dev libcurl4-openssl-dev python-setuptools python-dev python-pip python3-pip i2c-tools &>/dev/null
pip install inotify || exit
pip install pycurl
pip install progress
pip install python-gnupg
pip3 install inotify
pip3 install pycurl
pip3 install progress
pip3 install python-gnupg
echo "done."

baseurl="https://zk-sw-repo${mod}.s3.amazonaws.com"
# import zymbit gpg key
gpg_key_url="$baseurl/apt-zymkey-pubkey.gpg"
echo -n "Importing Zymbit Packages gpg key... "
# import the gpg key
curl -L "${gpg_key_url}" 2> /dev/null | apt-key add - &>/dev/null
echo "done."

# add zymbit apt repo to sources list
apt_source_path="/etc/apt/sources.list.d/zymbit.list"
echo -n "Installing $apt_source_path..."
distro=`lsb_release -c | cut -f2 -d$'\t'`
echo "deb $baseurl/apt-repo-${distro}/ ${distro} main" > $apt_source_path
echo "done...Updating now."
apt update -y

# install our packages
echo -n "Installing Zymkey Packages..."
apt install -y libzk libzymkeyssl zkbootrtc zkifc zkapputilslib zksaapps zkpkcs11 cryptsetup &>/dev/nul || exit
pip install -U zku &>/dev/null
pip install -U zk_luks &>/dev/null
pip3 install -U zku &>/dev/null
pip3 install -U zk_luks &>/dev/null
ln -s /usr/local/lib/python2.7/dist-packages/zk_luks/__init__.py /usr/local/bin/create_zk_crypt_vol
curl -G https://s3.amazonaws.com/zk-sw-repo/zk_prep_encr > /usr/local/bin/zk_prep_encr
chmod +x /usr/local/bin/zk_prep_encr

systemctl restart zkifc
sleep 10

# reboot
echo "Rebooting now..."
reboot
