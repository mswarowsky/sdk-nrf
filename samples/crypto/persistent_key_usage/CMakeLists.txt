#
# Copyright (c) 2021 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(persistent_key)

target_sources(app PRIVATE
        src/main.c
        )

target_sources_ifdef(CONFIG_TRUSTED_STORAGE app PRIVATE
        src/trusted_storage_init.c
        )

set_property(
        TARGET zephyr_property_target
        APPEND PROPERTY TFM_CMAKE_OPTIONS
        # Use -O0. So GDB reports correct line numbers.
        # -DCMAKE_BUILD_TYPE=Debug
        # NB: Probably not enough MPU regions on nrf53 in Isolation level 2
        # -DNULL_POINTER_EXCEPTION_DETECTION=ON
        # Add a debug function that logs the memory protection config, and
        # then invoke it from the SPE with log_memory_protection() to see
        # the memory protection configuration.
        #  -DLOG_MEMORY_PROTECTION=ON
        # WDT will force tests to continue for certain types of TF-M
        # tests. This can be inconvenient when debugging. But the WDT can
        # also be a necessary part of the test execution.
        # -DWATCHDOG_AVAILABLE=0
        )
