# 5-Way Switch Configuration

This document describes the implementation of 6-pin 5-out momentary switch support for the right side thumb cluster of the rickKlor keyboard.

## Hardware Configuration

### Pin Assignments
The 5-way switch is connected to the following pins on the right side:

- **Pin 0**: Up direction
- **Pin 14**: Down direction  
- **Pin 16**: Left direction
- **Pin 10**: Right direction
- **Pin 6**: Center/press

### Matrix Pin Changes
To accommodate the switch on pin 6, the keyboard matrix was modified:
- **Original**: Column pins 4, 5, 6, 7, 8
- **Updated**: Column pins 4, 5, 9, 7, 8 (moved pin 6 → pin 9)

## Software Implementation

### Files Modified

1. **`rickKlorRight-layouts.dtsi`**
   - Changed matrix column pin from 6 to 9
   - Added reference to right-side matrix transform
   - Updated chosen configuration for right side

2. **`rickKlorRight.overlay`**
   - Added `kscan_extra` for GPIO direct scanning of 5-way switch
   - Implemented `kscan_comp` composite kscan combining matrix and switch
   - Configured all 5 pins with pull-up resistors

3. **`rickKlor.dtsi`**
   - Added `right_transform` matrix transform for extended layout
   - Extended physical layout with 5 additional key positions
   - Maintained compatibility with left side configuration

4. **`rickKlor.keymap`**
   - Extended all layers to include 5-way switch bindings
   - Default layer: directional arrows + Enter
   - Raise layer: navigation keys (Page Up/Down, Home, End, Delete)
   - Layer 3: transparent bindings

### Technical Details

#### Composite kscan Configuration
```dts
kscan_comp: kscan_comp {
    compatible = "zmk,kscan-composite";
    label = "COMPOSITE_KSCAN";
    kscan = <&kscan0>, <&kscan_extra>;
};
```

#### GPIO Direct Scan for 5-Way Switch
```dts
kscan_extra: kscan_extra {
    compatible = "zmk,kscan-gpio-direct";
    label = "EXTRA_KEYS";
    
    input-gpios
        = <&pro_micro 0 (GPIO_ACTIVE_LOW | GPIO_PULL_UP)>    // up
        , <&pro_micro 14 (GPIO_ACTIVE_LOW | GPIO_PULL_UP)>   // down  
        , <&pro_micro 16 (GPIO_ACTIVE_LOW | GPIO_PULL_UP)>   // left
        , <&pro_micro 10 (GPIO_ACTIVE_LOW | GPIO_PULL_UP)>   // right
        , <&pro_micro 6 (GPIO_ACTIVE_LOW | GPIO_PULL_UP)>    // center
        ;
};
```

#### Matrix Transform
The right side uses an extended matrix transform to accommodate the additional keys:
- **Dimensions**: 6 rows × 10 columns (extended from 4×10)
- **Additional positions**: RC(4,0) through RC(4,4) for the 5-way switch

## Usage

### Default Layer
- **Switch Up**: UP arrow key
- **Switch Down**: DOWN arrow key
- **Switch Left**: LEFT arrow key
- **Switch Right**: RIGHT arrow key
- **Switch Center**: ENTER key

### Raise Layer (Layer 1)
- **Switch Up**: PAGE_UP
- **Switch Down**: PAGE_DOWN
- **Switch Left**: HOME
- **Switch Right**: END
- **Switch Center**: DELETE

### Layer 3
All switch positions are transparent (pass-through)

## Hardware Notes

### Switch Type
The implementation supports a standard 5-way tactile switch with the following characteristics:
- 6 pins total (5 outputs + 1 common)
- Momentary contact (non-latching)
- Active-low operation with internal pull-ups
- Standard 0.1" pin spacing

### Wiring
- All switch pins should be connected directly to the specified Pro Micro pins
- No external pull-up resistors needed (internal pull-ups enabled)
- Common pin can be connected to ground or left floating (GPIO configured as active-low)

### Debouncing
ZMK handles debouncing automatically for GPIO direct inputs. Default debounce settings should be sufficient for most mechanical switches.

## Troubleshooting

### If Keys Don't Respond
1. Check physical connections to pins 0, 6, 10, 14, 16
2. Verify switch orientation and pin mapping
3. Test switch continuity with multimeter
4. Check for conflicts with other pin usage

### If Some Directions Don't Work
1. Verify correct pin assignments in hardware
2. Check for cold solder joints
3. Test individual switch directions for mechanical issues

### Build Issues
1. Ensure all syntax is correct in modified files
2. Check for balanced braces and proper device tree syntax
3. Verify composite kscan configuration is supported in ZMK version

## Compatible Hardware

This configuration is designed for:
- Nice Nano v2 controller
- Standard 5-way tactile switches (e.g., Alps SKQUCAA010)
- Pro Micro compatible boards with same pin layout

## Future Enhancements

Potential improvements for future versions:
- Configurable switch behaviors per layer
- Scroll wheel emulation using directional inputs
- Hold-tap behaviors for modifier combinations
- Custom behaviors for joystick-like navigation