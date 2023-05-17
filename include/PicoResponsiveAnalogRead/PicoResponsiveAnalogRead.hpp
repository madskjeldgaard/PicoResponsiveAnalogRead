#pragma once

#include <cmath>
#include <cstdint>

#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"

/*
 * This is a port of the PicoResponsiveAnalogRead library for Arduino to make it
 * work with the Raspberry Pi Pico Ported by Mads Kjeldgaard. Original by Damien
 * Clarke.
 *
 */

namespace picoresponsiveanalog
{
class PicoResponsiveAnalogRead
{
public:
  // pin - the pin to read
  // sleepEnable - enabling sleep will cause values to take less time to stop
  // changing and potentially stop changing more abruptly,
  //   where as disabling sleep will cause values to ease into their correct
  //   position smoothly
  // snapMultiplier - a value from 0 to 1 that controls the amount of easing
  //   increase this to lessen the amount of easing (such as 0.1) and make the
  //   responsive values more responsive but doing so may cause more noise to
  //   seep through if sleep is not enabled

  PicoResponsiveAnalogRead() {};  // default constructor must be followed by
                                  // call to begin function
  PicoResponsiveAnalogRead(int gpioPin,
                           int adcNum,
                           bool sleepEnable,
                           float snapMultiplier = 0.01)
  {
    // Make sure the pin is one of the two ADC pins (26 or 27)
    // static_assert((pin == 26) || (pin == 27), "Pin must be 26 or 27 (the only
    // two ADC pins on the Pico)");

    begin(gpioPin, adcNum, sleepEnable, snapMultiplier);
  };

  void begin(int gpioPin,
             int adcNum,
             bool sleepEnable,
             float snapMultiplier = 0.01)
  {
    // Setup pin
    this->adc_input = adcNum;
    this->gpioPin = gpioPin;
    this->sleepEnable = sleepEnable;
    adc_gpio_init(gpioPin);
    setSnapMultiplier(snapMultiplier);
  };  // use with default constructor to initialize

  inline auto getValue() const
  {
    return responsiveValue;
  }  // get the responsive value from last update
  inline auto getRawValue() const
  {
    return rawValue;
  }  // get the raw analogRead() value from last update
  inline auto hasChanged() const
  {
    return responsiveValueHasChanged;
  }  // returns true if the responsive value has changed during the last update
  inline auto isSleeping() const
  {
    return sleeping;
  }  // returns true if the algorithm is currently in sleeping mode

  auto snapCurve(float x)
  {
    float y = 1.0f / (x + 1.0f);
    y = (1.0f - y) * 2.0f;
    if (y > 1.0f) {
      return 1.0f;
    }
    return y;
  };

  auto getResponsiveValue(int newValue)
  {
    // if sleep and edge snap are enabled and the new value is very close to an
    // edge, drag it a little closer to the edges This'll make it easier to pull
    // the output values right to the extremes without sleeping, and it'll make
    // movements right near the edge appear larger, making it easier to wake up
    if (sleepEnable && edgeSnapEnable) {
      if (newValue < activityThreshold) {
        newValue = (newValue * 2) - activityThreshold;
      } else if (newValue > analogResolution - activityThreshold) {
        newValue = (newValue * 2) - analogResolution + activityThreshold;
      }
    }

    // get difference between new input value and current smooth value
    unsigned int diff = std::abs(newValue - smoothValue);

    // measure the difference between the new value and current value
    // and use another exponential moving average to work out what
    // the current margin of error is
    errorEMA += ((newValue - smoothValue) - errorEMA) * 0.4;

    // if sleep has been enabled, sleep when the amount of error is below the
    // activity threshold
    if (sleepEnable) {
      // recalculate sleeping status
      sleeping = abs(errorEMA) < activityThreshold;
    }

    // if we're allowed to sleep, and we're sleeping
    // then don't update responsiveValue this loop
    // just output the existing responsiveValue
    if (sleepEnable && sleeping) {
      return static_cast<int>(smoothValue);
    }

    // use a 'snap curve' function, where we pass in the diff (x) and get back a
    // number from 0-1. We want small values of x to result in an output close
    // to zero, so when the smooth value is close to the input value it'll
    // smooth out noise aggressively by responding slowly to sudden changes. We
    // want a small increase in x to result in a much higher output value, so
    // medium and large movements are snappy and responsive, and aren't made
    // sluggish by unnecessarily filtering out noise. A hyperbola (f(x) = 1/x)
    // curve is used. First x has an offset of 1 applied, so x = 0 now results
    // in a value of 1 from the hyperbola function. High values of x tend toward
    // 0, but we want an output that begins at 0 and tends toward 1, so 1-y
    // flips this up the right way. Finally the result is multiplied by 2 and
    // capped at a maximum of one, which means that at a certain point all
    // larger movements are maximally snappy

    // then multiply the input by SNAP_MULTIPLER so input values fit the snap
    // curve better.
    float snap = snapCurve(diff * snapMultiplier);

    // when sleep is enabled, the emphasis is stopping on a responsiveValue
    // quickly, and it's less about easing into position. If sleep is enabled,
    // add a small amount to snap so it'll tend to snap into a more accurate
    // position before sleeping starts.
    if (sleepEnable) {
      snap *= 0.5 + 0.5;
    }

    // calculate the exponential moving average based on the snap
    smoothValue += (newValue - smoothValue) * snap;

    // ensure output is in bounds
    if (smoothValue < 0.0) {
      smoothValue = 0.0;
    } else if (smoothValue > analogResolution - 1) {
      smoothValue = analogResolution - 1;
    }

    // expected output is an integer
    return static_cast<int>(smoothValue);
  };

  void update()
  {
    // Conversion factor (convert the adc input from 3.3volt 12 bit)

    // Select ADC input 0 (GPIO26)
    adc_select_input(adc_input);

    rawValue = adc_read();
    this->update(rawValue);
  };  // updates the value by performing an analogRead() and
      // calculating a responsive value based off it
  void update(int rawValueRead)
  {
    rawValue = rawValueRead;
    prevResponsiveValue = responsiveValue;
    responsiveValue = getResponsiveValue(rawValue);
    responsiveValueHasChanged = responsiveValue != prevResponsiveValue;
  };  // updates the value accepting a value and
      // calculating a responsive value based off it

  void setSnapMultiplier(float newMultiplier)
  {
    if (newMultiplier > 1.0) {
      newMultiplier = 1.0;
    }
    if (newMultiplier < 0.0) {
      newMultiplier = 0.0;
    }
    snapMultiplier = newMultiplier;
  };
  inline void enableSleep()
  {
    sleepEnable = true;
  }
  inline void disableSleep()
  {
    sleepEnable = false;
  }
  inline void enableEdgeSnap()
  {
    edgeSnapEnable = true;
  }
  // edge snap ensures that values at the edges of the spectrum (0 and 1023) can
  // be easily reached when sleep is enabled
  inline void disableEdgeSnap()
  {
    edgeSnapEnable = false;
  }
  inline void setActivityThreshold(float newThreshold)
  {
    activityThreshold = newThreshold;
  }
  // the amount of movement that must take place to register as activity and
  // start moving the output value. Defaults to 4.0
  inline void setAnalogResolution(int resolution)
  {
    analogResolution = resolution;
  }
  // if your ADC is something other than 12bit (4096), set that here

private:
  int gpioPin;
  int analogResolution = 4096;
  float snapMultiplier;
  bool sleepEnable;
  float activityThreshold = 4.0;
  bool edgeSnapEnable = true;

  float smoothValue;
  unsigned long lastActivityMS;
  float errorEMA = 0.0;
  bool sleeping = false;

  int rawValue;
  int responsiveValue;
  int prevResponsiveValue;
  bool responsiveValueHasChanged;

  int adc_input;
};

}  // namespace picoresponsiveanalog
