# Pimoroni Trackball Configuration (REMOVED)

**Note: The Pimoroni Trackball configuration has been removed to fix the build.**

The previous configuration attempted to use ZMK pointing device features that are not available in ZMK v0.3. The build was failing because:

1. `CONFIG_ZMK_POINTING_DEVICE` is not available in ZMK v0.3
2. The `pimoroni,trackball` device tree binding doesn't exist in ZMK
3. ZMK v0.3 doesn't have built-in pointing device support

## What was removed

1. **config/boards/shields/rickKlor/rickKlorRight.overlay**
   - Removed I2C configuration and pinctrl settings
   - Removed trackball device definition
   - Removed chosen pointer device

2. **config/boards/shields/rickKlor/rickKlorRight.conf**
   - Removed `CONFIG_I2C=y`
   - Removed `CONFIG_ZMK_POINTING_DEVICE=y` and related configs

3. **config/dts/bindings/sensor/pimoroni,trackball.yaml**
   - Removed the custom device tree binding file

## Future considerations

For trackball support, you would need either:
1. Wait for ZMK to officially support pointing devices
2. Use a custom ZMK fork with pointing device support
3. Implement a custom solution using available ZMK sensor APIs

The build should now work correctly without the trackball functionality.