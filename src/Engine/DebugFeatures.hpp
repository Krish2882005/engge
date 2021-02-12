#pragma once

namespace ng {

struct DebugFeatures {
  inline static bool showHoveredObject{false};
  inline static bool showCursorPosition{false};
  inline static bool showTextBounds{false};
  inline static ngf::TimeSpan _renderTime;
  inline static ngf::TimeSpan _updateTime;
};

}