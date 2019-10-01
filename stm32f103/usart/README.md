# UART tx example
This is an example of write only UART.
This is useful when UART is used for debugging purpose where UART do not need to read information.

# Note: Compilers
The arm-none-eabi-gcc from Fedora (as of arm-none-eabi-gcc-cs-9.2.0-1.fc30.x86_64) do not support printf way. So this need to be built use the [GNU ARM Embedded toolchain from ARM](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads).