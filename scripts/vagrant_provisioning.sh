#!/usr/bin/env bash

apt-get update
apt-get install -y apache2
if ! [ -L /var/www ]; then
  rm -rf /var/www
  ln -fs /vagrant /var/www
fi
apt-get install libpthread-stubs0-dev
wget http://www.biicode.com/downloads/latest/ubuntu64
mv ubuntu64 bii-ubuntu64.deb
dpkg -i bii-ubuntu64.deb && apt-get -f install
rm bii-ubuntu64.deb
wget https://s3.amazonaws.com/biibinaries/thirdparty/cmake-3.0.2-Linux-64.tar.gz
tar -xzf cmake-3.0.2-Linux-64.tar.gz
cp -fR cmake-3.0.2-Linux-64/* /usr
rm -rf cmake-3.0.2-Linux-64
