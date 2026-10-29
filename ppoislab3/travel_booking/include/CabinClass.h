#pragma once
#include <string>

enum class CabinClass { Economy, Business };

inline std::string toString(CabinClass cabin) { return cabin == CabinClass::Economy ? "Эконом" : "Бизнес"; }
inline double priceMultiplier(CabinClass cabin) { return cabin == CabinClass::Economy ? 1.0 : 2.5; }
