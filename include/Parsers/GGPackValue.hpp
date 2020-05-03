#pragma once
#include <map>
#include <fstream>
#include <string>
#include <vector>

namespace ng {
struct GGPackValue {
  char type;
  std::string string_value;
  int int_value{0};
  double double_value{0};
  std::map<std::string, GGPackValue> hash_value;
  std::vector<GGPackValue> array_value;
  static GGPackValue nullValue;

public:
  GGPackValue();
  GGPackValue(const GGPackValue &value);

  [[nodiscard]] bool isNull() const;
  [[nodiscard]] bool isHash() const;
  [[nodiscard]] bool isArray() const;
  [[nodiscard]] bool isString() const;
  [[nodiscard]] bool isInteger() const;
  [[nodiscard]] bool isDouble() const;

  GGPackValue &operator[](size_t index);
  const GGPackValue &operator[](size_t index) const;
  GGPackValue &operator[](const std::string &key);
  const GGPackValue &operator[](const std::string &key) const;
  GGPackValue &operator=(const GGPackValue &other);
  virtual ~GGPackValue();

  [[nodiscard]] int getInt() const;
  [[nodiscard]] double getDouble() const;
  [[nodiscard]] std::string getString() const;

  template<typename T>
  static GGPackValue toGGPackValue(T value);

  friend std::ostream &operator<<(std::ostream &os, const GGPackValue &value);
};
}