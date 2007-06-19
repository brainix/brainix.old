#!/usr/bin/bash

sh compile.sh
sudo sh update-bootdisk.sh
bochs -f dot-bochsrc
