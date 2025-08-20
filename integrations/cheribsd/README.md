CheriBSD Integration
====================

This model for the test suite builds the tests as a CheriBSD userspace process.

```console
$ mkdir build
$ cd build
$ cmake ..
```

You can then build simply by running `make`.
If you are cross compiling, then you will need to provide a cross-compile CMake toolchain, such as the one provided by cheribuild.

The build will produce a single program: `conformance-tests`
Run this binary on your CheriBSD system.
