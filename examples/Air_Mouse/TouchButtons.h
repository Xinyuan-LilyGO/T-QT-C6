#pragma once

#include <Arduino.h>

#include "TouchDevice.h"

namespace input {

enum class MouseButton : uint8_t { None, Left, Right };
enum class ButtonEventType : uint8_t { Pressed, Released };

struct ButtonEvent {
  ButtonEventType type;
  MouseButton button;
};

class TouchButtons {
 public:
  bool update(const drivers::TouchPoint& touch, uint32_t nowMs,
              ButtonEvent& event);
  void reset();

 private:
  static constexpr uint32_t kDebounceMs = 24;
  static constexpr uint16_t kSplitX = 64;

  MouseButton classify(const drivers::TouchPoint& touch) const;

  MouseButton stable_{MouseButton::None};
  MouseButton candidate_{MouseButton::None};
  uint32_t candidateSinceMs_{0};
};

}  // namespace input
