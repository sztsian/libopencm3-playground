# gd32f130-playground
GD32F130 playground using [LibOpenCM3](https://github.com/libopencm3/libopencm3). 

Code are mostly for my [STM32F0/GD32F1 multi function board](https://github.com/sztsian/boards-kicad/tree/master/STM32-tssop20board)

# Notes about compiling
After clone this repo, init the sub module

> git submodule init

And then compile LibOpenCM3

> pushd libopencm3 ; make ; popd

Then write code and make like ordinary programs.

# Reference

* [LibOpenCM3 Examples](https://github.com/libopencm3/libopencm3-examples)
* [Embedded ARM Part 0: STM32 programming with libopencm3](https://rhye.org/post/stm32-with-opencm3-0-compiling-and-uploading/) by [Ross Schlaikjer](https://github.com/rschlaikjer)
