#!/bin/bash

#VARIABLES
op=$1
os=$2

#este es pa ubuntu

sudo apt-get install aptitude

#Estas son las dependencias descritas en el README.rst
sudo apt-get install openssl libgmp3-dev libgmp-dev libnettle4 libxml2 libxml2-dev zlib-bin zlibc zlib1g zlib1g-dev libghc-zlib-bindings-dev libghc-zlib-dev libc-ares-dev libc-ares2 libsqlite3-dev libsqlite3-0 expat libexpat-dev

# estas son las dependencias que vienen mas abajo
sudo apt-get install libgnutls-dev gnutls-bin gnutls-dev libgmp-dev libc-ares-dev pkg-config libgpg-error-dev libgcrypt-dev libssl-dev 

# LAS OTRAS dependencias

sudo apt-get install libtorrent-dev libcppunit-1.12-1 libcppunit-dev automake autoconf autoconf-archive gettext gettext-base libgettextpo-dev autopoint libtool

autoreconf -fiv >compilacion.log

./configure >>compilacion.log

make >>compilacion.log

sudo make install >>compilacion.log
