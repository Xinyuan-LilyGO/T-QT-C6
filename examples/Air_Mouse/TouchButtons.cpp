#include "TouchButtons.h"

namespace input {

bool TouchButtons::update(const drivers::TouchPoint& touch, uint32_t nowMs,
                          ButtonEvent& event) {
  MouseButton raw = classify(touch);

  // Once pressed, crossing the center line cannot change the selected button.
  if (stable_ != MouseButton::None && touch.down) {
    raw = stable_;
  }

  if (raw != candidate_) {
    candidate_ = raw;
    candidateSinceMs_ = nowMs;
    return false;
  }

  if (candidate_ == stable_ || nowMs - candidateSinceMs_ < kDebounceMs) {
    return false;
  }

  if (stable_ != MouseButton::None) {
    event = {ButtonEventType::Released, stable_};
    stable_ = MouseButton::None;
    return true;
  }

  stable_ = candidate_;
  event = {ButtonEventType::Pressed, stable_};
  return true;
}

void TouchButtons::reset() {
  stable_ = MouseButton::None;
  candidate_ = MouseButton::None;
  candidateSinceMs_ = millis();
}

MouseButton TouchButtons::classify(const drivers::TouchPoint& touch) const {
  if (!touch.down) {
    return MouseButton::None;
  }
  return touch.x < kSplitX ? MouseButton::Left : MouseButton::Right;
}

}  // namespace input
