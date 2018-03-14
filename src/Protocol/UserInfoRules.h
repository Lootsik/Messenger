#pragma once
#include <string>

extern const char AllowedSymbols[];

bool IsAllowedLogin(const std::string& Login);

bool IsAllowedPassword(const std::string& Password);

std::string PasswordHash(const std::string& Password);