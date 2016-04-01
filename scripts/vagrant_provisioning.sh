#!/usr/bin/env bash

apt-get update
apt-get install -y apache2
if ! [ -L /var/www ]; then
  rm -rf /var/www
  ln -fs /vagrant /var/www
fi

#Installing development independencies
apt-get -y install git-all
apt-get -y install vim
apt-get -y install screen

# install node.js the usual way (will also install npm this time)
sudo apt-get install -y nodejs

#Installing application independencies
set -x #Output progress
apt-get -y install build-essential
apt-get -y install libpthread-stubs0-dev
wget --quiet http://www.biicode.com/downloads/latest/ubuntu64
mv ubuntu64 bii-ubuntu64.deb
dpkg -i bii-ubuntu64.deb && apt-get -f install
rm bii-ubuntu64.deb
wget --quiet https://s3.amazonaws.com/biibinaries/thirdparty/cmake-3.0.2-Linux-64.tar.gz
tar -xzf cmake-3.0.2-Linux-64.tar.gz
cp -fR cmake-3.0.2-Linux-64/* /usr
rm -rf cmake-3.0.2-Linux-64
rm cmake-3.0.2-Linux-64.tar.gz

#Node stuff

#PPA from nodesource
curl -sL https://deb.nodesource.com/setup | sudo bash -
apt-get -y install nodejs

set +x #Stop outputting progress
