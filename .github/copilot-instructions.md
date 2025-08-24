# RickKlor ZMK Firmware

RickKlor is a custom split mechanical keyboard firmware built using ZMK (Zephyr Mechanical Keyboard). This repository contains the keyboard configuration and build definitions for the RickKlor split keyboard that runs on nice_nano_v2 controllers.

Always reference these instructions first and fallback to search or bash commands only when you encounter unexpected information that does not match the info here.

## Working Effectively

### Bootstrap and Build Environment Setup
- Install Python 3.12+: `python3 --version` (already available in most environments)
- Install West tool: `python3 -m pip install --upgrade pip && python3 -m pip install west`
- Add West to PATH: `export PATH=$HOME/.local/bin:$PATH`
- Initialize workspace: `cd /path/to/rick-klor && west init -l config`
- Download ZMK source: `west update` -- takes 2-3 minutes for git clone. NEVER CANCEL. Set timeout to 600+ seconds.
- Export Zephyr environment: `west zephyr-export`

### Install Zephyr SDK (Required for Building)
- Download SDK: `cd /tmp && wget https://github.com/zephyrproject-rtos/sdk-ng/releases/download/v0.16.8/zephyr-sdk-0.16.8_linux-x86_64.tar.xz`
- Extract: `tar -xf zephyr-sdk-0.16.8_linux-x86_64.tar.xz`
- Install: `cd zephyr-sdk-0.16.8 && yes | ./setup.sh` -- takes 1-2 minutes. NEVER CANCEL. Set timeout to 300+ seconds.
- Install Python dependencies: `pip3 install -r zephyr/scripts/requirements.txt` -- takes 3-5 minutes. NEVER CANCEL. Set timeout to 600+ seconds.

### Build Firmware
- **Build left half**: `west build -p -s zmk/app -b nice_nano_v2 -- -DSHIELD=rickKlorLeft -DZMK_CONFIG=/path/to/rick-klor/config` -- takes ~50 seconds. NEVER CANCEL. Set timeout to 300+ seconds.
- **Build right half**: `west build -p -s zmk/app -b nice_nano_v2 -- -DSHIELD=rickKlorRight -DZMK_CONFIG=/path/to/rick-klor/config` -- takes ~50 seconds. NEVER CANCEL. Set timeout to 300+ seconds.
- **Build settings reset**: `west build -p -s zmk/app -b nice_nano_v2 -- -DSHIELD=settings_reset -DZMK_CONFIG=/path/to/rick-klor/config` -- takes ~50 seconds. NEVER CANCEL. Set timeout to 300+ seconds.
- Build outputs are in `build/zephyr/`: `zmk.uf2` (main firmware file), `zmk.hex`, `zmk.bin`

### Build Validation Steps
- Always ensure all builds complete without errors
- Check that `zmk.uf2` files are generated in `build/zephyr/`
- Verify memory usage is reasonable (typically <30% flash, <20% RAM)
- Test all three shield variants: rickKlorLeft, rickKlorRight, settings_reset

## Validation

### Manual Testing Requirements
- **ALWAYS** build all three firmware variants after making changes
- **ALWAYS** verify the keymap syntax is valid by successful compilation
- **ALWAYS** check that ZMK Studio features are enabled (CONFIG_ZMK_STUDIO=y in rickKlor.conf)
- You cannot physically test the keyboard functionality, but you can validate the firmware builds correctly
- Verify that the GitHub Actions workflow will pass by testing all build matrix entries locally

### Common Validation Commands
- Check repository structure: `ls -la` in repo root shows: build.yaml, config/, zephyr/, zmk/, .github/
- Verify keymap syntax: Build succeeds without errors
- Check build outputs: `ls -la build/zephyr/zmk.*` shows .uf2, .hex, .bin files

## Repository Structure and Key Files

### Essential Files for Development
- `config/boards/shields/rickKlor/rickKlor.keymap` - Main keymap configuration with layers and key bindings
- `config/boards/shields/rickKlor/rickKlor.conf` - Feature configurations (Studio, sleep, Bluetooth settings)
- `config/boards/shields/rickKlor/rickKlor.dtsi` - Hardware definition and matrix layout
- `config/boards/shields/rickKlor/rickKlorLeft.overlay` - Left half pin configuration
- `config/boards/shields/rickKlor/rickKlorRight.overlay` - Right half pin configuration
- `build.yaml` - GitHub Actions build matrix defining all build variants
- `.github/workflows/build.yml` - CI workflow (uses ZMK's standard build process)

### Repository Root Structure
```
.
├── .github/workflows/build.yml    # CI workflow
├── build.yaml                     # Build matrix configuration
├── config/                        # ZMK configuration
│   ├── west.yml                   # West manifest pointing to ZMK v0.3
│   ├── rickKlor_zmk.yml          # Shield metadata
│   └── boards/shields/rickKlor/   # Keyboard definitions
├── zephyr/                        # Downloaded by west update
└── zmk/                           # Downloaded by west update
```

### Key Configuration Areas
- **Keymap editing**: Modify `config/boards/shields/rickKlor/rickKlor.keymap`
- **Feature toggles**: Edit `config/boards/shields/rickKlor/rickKlor.conf`
- **Hardware changes**: Update `.overlay` and `.dtsi` files
- **Build targets**: Modify `build.yaml` for CI changes

## Build System Details

### Build Process
- ZMK uses Zephyr RTOS build system with CMake
- West tool manages multi-repository workspace
- Builds target nice_nano_v2 board (nRF52840 wireless microcontroller)
- Each shield (left/right/reset) creates separate firmware files

### Build Timing Expectations
- Initial setup (SDK download + install): 5-10 minutes total
- West update (first time): 2-3 minutes
- Each firmware build: ~50 seconds
- **CRITICAL**: Builds are FAST for this repository type. Do not expect 45+ minute builds.

### CI Integration
- GitHub Actions automatically builds all variants on push/PR
- Uses ZMK's official build workflow: `zmkfirmware/zmk/.github/workflows/build-user-config.yml@v0.3`
- No additional validation beyond successful compilation required

## Common Development Tasks

### Making Keymap Changes
1. Edit `config/boards/shields/rickKlor/rickKlor.keymap`
2. Build left half to validate syntax: `west build -p -s zmk/app -b nice_nano_v2 -- -DSHIELD=rickKlorLeft -DZMK_CONFIG=/path/to/config`
3. Build right half: `west build -p -s zmk/app -b nice_nano_v2 -- -DSHIELD=rickKlorRight -DZMK_CONFIG=/path/to/config`
4. Verify both builds succeed without errors

### Adding New Features
1. Check feature availability in ZMK documentation
2. Add CONFIG_ options to `config/boards/shields/rickKlor/rickKlor.conf`
3. Update keymap if needed
4. Build and validate all variants

### Hardware Configuration Changes
1. Modify `.overlay` files for pin assignments
2. Update `.dtsi` for matrix or layout changes
3. Test all build variants
4. Verify memory usage remains reasonable

## Troubleshooting

### Common Build Issues
- **"CMakeLists.txt not found"**: Use `-s zmk/app` to specify source directory
- **Shield not found**: Verify SHIELD name matches files in `config/boards/shields/rickKlor/`
- **Config path issues**: Use absolute path for `-DZMK_CONFIG`
- **Memory overflow**: Check feature configurations, disable unused features

### Environment Issues
- **West not found**: Add `$HOME/.local/bin` to PATH
- **SDK issues**: Re-run SDK setup.sh with `yes | ./setup.sh`
- **Permission errors**: Use `--user` flag with pip installations

Always work from the repository root directory for all West commands.