# Hey! I'm Filing Here

**Program that generates an image for a mountable 1 MiB ext2 file system**

## Building

To build the program, type
`make`

## Running

To run the executable to create cs111-base.img, type
`./ext2-create` 

To mount, create a directory for the filesystem
`mkdir mnt`

To mount the filesystem, type
`sudo mount -o loop cs111-base.img mnt`

To check if the filesystem is correct, type
`cd mnt; ls -ain` 

Your output should look something similar to:

    total 7

      2 drwxr-xr-x 3    0    0 1024 Mar  9 20:48 .

    374 drwxr-xr-x 4 1000 1000 4096 Mar  9 20:43 ..

     13 lrw-r--r-- 1 1000 1000   11 Mar  9 20:48 hello -> hello-world

     12 -rw-r--r-- 1 1000 1000   12 Mar  9 20:48 hello-world

     11 drwxr-xr-x 2    0    0 1024 Mar  9 20:48 lost+found

(Times and second row might be a bit different)

## Cleaning up

To unmount the filesystem, exit the filesystem and type 
`sudo umount mnt`

Remove the directory used for mounting
`rmdir mnt`

To clean up the program and object files, type
`make clean`
