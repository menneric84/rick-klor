# Copyright (c) 2023-2025 The ZMK Contributors
# SPDX-License-Identifier: MIT

# Main CMakeLists.txt for rickKlor custom drivers

if(CONFIG_ZMK_TRACKBALL_PIM447)
  add_subdirectory(drivers/sensor/trackball_pim447)
endif()