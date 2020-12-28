#pragma once
#include <ngf/Graphics/Color.h>
#include <ngf/Graphics/Colors.h>

namespace ng {
struct _ControlConstants {
  inline static const ngf::Color NormalColor{ngf::Colors::White};
  inline static const ngf::Color DisabledColor{255, 255, 255, 128};
  inline static const ngf::Color HoveColor{ngf::Colors::Yellow};
};
}
