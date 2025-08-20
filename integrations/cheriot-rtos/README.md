CHERIoT RTOS Integration
========================

This model for the test suite builds the tests as a CHERIoT firmware image using CHERIoT RTOS.
The RTOS sources must be provided by the `CHERIOT_SDK` environment variable.
You can configure the build with:

```console
$ CHERIOT_SDK=/path/to/rtos/sdk xmake config --sdk=/path/to/toolchain --board=your-board-name
```

You can then build simply by running `xmake`.
If your board has a run command configured, you can then run the test suite with `xmake run`, otherwise you must manually deploy the firmware image to your test target.
