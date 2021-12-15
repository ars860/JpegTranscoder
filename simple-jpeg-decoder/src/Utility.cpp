#include "Utility.hpp"

bool DEBUG = false;

std::ofstream logFile = DEBUG ? std::ofstream("kpeg.log", std::ios::out) : std::ofstream();