#pragma once

#include <csignal>
#include <functional>
#include <unordered_map>

class SignalHandler {
public:
    static void register_signal_handler(int sig, std::function<void()> handler) {
        s_handlers[sig] = std::move(handler);
        ::signal(sig, &signal_handler);
    }

private:
    static void signal_handler(int sig) {
        s_handlers[sig]();
    }

private:
    static std::unordered_map<int, std::function<void()>>
        s_handlers;  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
};
