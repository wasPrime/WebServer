#pragma once

inline constexpr const char* LOCAL_HOST = "127.0.0.1";
inline constexpr int PORT = 8765;

void errif(bool condition, const char* errmsg);
