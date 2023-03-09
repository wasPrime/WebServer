#include "SignalHandler.h"

std::unordered_map<int, std::function<void()>> SignalHandler::s_handlers;
