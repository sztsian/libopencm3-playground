# LibOpenCM3-playground
[LibOpenCM3](https://github.com/libopencm3/libopencm3) playground. 

# Notes about compiling
After clone this repo, init the sub module

> git submodule init; git submodule update

And then compile LibOpenCM3

> pushd libopencm3 ; make ; popd

If you want to have debug symbols, you should 

> pushd libopencm3 ; CFLAGS="-g" make ; popd

Then write code and make like ordinary programs.

# Reference

* [LibOpenCM3 Examples](https://github.com/libopencm3/libopencm3-examples)
* [Embedded ARM Part 0: STM32 programming with libopencm3](https://rhye.org/post/stm32-with-opencm3-0-compiling-and-uploading/) by [Ross Schlaikjer](https://github.com/rschlaikjer)
