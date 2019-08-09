#!/bin/bash -x

sudo chmod -R 777 .
sudo ulimit;ulimit -a
./my_format floppy.img
mkdir mntpoint
sudo mount -o loop,uid=user,gid=user floppy.img mntpoint/
mount | grep mntpoint 
mount | grep mntpoint | grep fat 
for ((a=0; a < 110 ; a++))
do
   mkdir mntpoint/dir_$a
   echo "Hello world" > mntpoint/dir_$a/foo

done

ls -altrh mntpoint/ | wc -l

sudo umount mntpoint/

sudo mount -o loop,uid=user,gid=user floppy.img mntpoint/
ls -altrh mntpoint/
ls -altrh mntpoint/ | wc -l
cat mntpoint/*/foo 
./my_format floppy.img 
sudo umount mntpoint
