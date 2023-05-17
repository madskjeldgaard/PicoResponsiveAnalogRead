# PicoResponsiveAnalogRead

This is a small library for making analog readings with less noise on the Raspberry Pi Pico, using the [Pico C++ SDK](https://github.com/raspberrypi/pico-sdk). It is a port of [ResponsiveAnalogRead](https://github.com/dxinteractive/ResponsiveAnalogRead) by Damien Clarke.

## Example

For an example on how to use this project, see [this](https://github.com/madskjeldgaard/SimplePicoMidiController).

## Adding this library to your project

Note: For this library to work, you need to include the `hardware_adc` library from the Pico SDK. See below.

### Using CPM
The easiest way to use this in your project is to install [it using CPM](https://github.com/cpm-cmake/CPM.cmake). Add the following lines to your CMakeLists.txt. This will automatically download and include it in your CMake setup.

```cmake
# Add Debounce
CPMAddPackage("gh:madskjeldgaard/PicoResponsiveAnalogRead#main")
target_link_libraries(SimplePicoMidiController PicoResponsiveAnalogRead::PicoResponsiveAnalogRead)
target_link_libraries(SimplePicoMidiController hardware_adc) # Dependency for PicoResponsiveAnalogRead
```

### Manually
If you have a project called `SimplePicoMidiController`, you can link this library in your CMakeLists.txt like so (assuming you downloaded the library and placed it in the path noted below):  

```cmake
# Add PicoResponsiveAnalogRead
set(PICO_READ "include/PicoResponsiveAnalogRead")
add_subdirectory(${PICO_READ})
target_link_libraries(SimplePicoMidiController PicoResponsiveAnalogRead::PicoResponsiveAnalogRead)
target_link_libraries(SimplePicoMidiController hardware_adc) # Dependency for PicoResponsiveAnalogRead
```

# Contributing

See the [CONTRIBUTING](CONTRIBUTING.md) document.
