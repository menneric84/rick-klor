# Pimoroni Trackball Configuration

This firmware includes support for the Pimoroni Trackball on the right half of the rickKlor keyboard.

## Hardware Connection

The trackball should be connected to the following pins on the right half:

- **SDA (Data)**: Pin P0.17  
- **SCL (Clock)**: Pin P0.20
- **VCC**: 3.3V
- **GND**: Ground

## Configuration

### I2C Settings
- **Address**: 0x0a (default Pimoroni Trackball address)
- **Clock Speed**: 100kHz
- **Interface**: I2C0

### Pin Assignments
The selected pins (P0.17, P0.20) for I2C communication:
- Keyboard matrix uses pins: 4, 5, 6, 7, 8 (columns) and 15, 18, 19, 20 (rows)
- Trackball uses pins: 17 (SDA), 20 (SCL)

**Note**: Pin P0.20 is shared with one of the keyboard matrix rows. Ensure this doesn't conflict with your specific hardware implementation.

## Files Modified

1. **config/boards/shields/rickKlor/rickKlorRight.overlay**
   - Added I2C configuration and pinctrl settings
   - Defined trackball device at I2C address 0x0a
   - Configured chosen pointer device

2. **config/boards/shields/rickKlor/rickKlorRight.conf**
   - Enabled I2C support: `CONFIG_I2C=y`
   - Enabled pointing device: `CONFIG_ZMK_POINTING_DEVICE=y`

3. **config/dts/bindings/sensor/pimoroni,trackball.yaml**
   - Device tree binding for Pimoroni Trackball
   - Defines compatible interface and properties

## Features

- Mouse movement via trackball
- Configurable X/Y axis inversion
- Adjustable resolution (default: 400 CPI)
- Integration with ZMK pointing device system

## Usage

Once flashed, the trackball will function as a mouse input device. Movement will be transmitted from the right half to the left half (central) via the split keyboard connection.

## Troubleshooting

- Ensure I2C connections are secure
- Verify 3.3V power supply to trackball  
- Check that the trackball address (0x0a) is correct
- Confirm no pin conflicts with other peripherals