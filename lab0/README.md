# A Kernel Seedling

**Counts the number of running processes and inserts count into /proc/count**

## Building

To build the module, type

    make

## Running

To run the module, insert the module into the kernel with

    sudo insmod proc_count.ko

To see the number of running processes, type
    
    cat /proc/count

## Cleaning Up

To remove the kernel module and clean up the code, type

    sudo rmmod proc_count

## Testing

Tested module on kernel release 5.14.8-arch1-1