#!/usr/bin/bash

sudo mount -o loop Bootdisk.img ../mnt/
sudo cp ./bin/Brainix ../mnt
sudo rm ../mnt/Brainix.gz
sudo gzip ../mnt/Brainix
sudo umount ../mnt/
