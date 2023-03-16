#pragma once

#include <ip4.hpp>
#include <string>
#include <optional>

std::string ip_to_str_code(const ip4& ip);

std::optional<ip4> str_code_to_ip(const std::string& str);
