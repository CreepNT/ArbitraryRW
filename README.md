#ArbitraryRW
Kernel module + library to allow userland modules to write on RX pages.

#Usage
Include `ArbitraryRW.h` in your project, and link against `libArbitraryRW_stub.a` / `libArbitraryRW_stub_weak.a`.
(For CMake users : add `ArbitraryRW_stub` or `ArbitraryRW_stub_weak` to the `target_link_libraries`, and add the folder in which the files are to `link_directories`)

If your module **requires** ArbitraryRW, it is recommended to link against the regular stubs, as such the module will not load if the kernel module is not loaded.
Linking against the weak stubs ensures your module will start even if the ArbitraryRW kernel module is not loaded (all imported functions will return -1).