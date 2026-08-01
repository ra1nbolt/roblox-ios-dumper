//
// Created by ra1nbolt on 5/20/26.
//

#pragma once

#include <string>
#include <map>

namespace signatures {
    enum class resolve_type {
        none,
        bl,
        ldr,
        adrl,
    };

    struct sig_info {
        std::string pattern;
        resolve_type resolve;
    };

    enum category {
        none,
        lua,
        rbx,
    };

    constexpr std::string_view cat2str(category cat) {
        switch (cat) {
            case category::lua: return "luau";
            case category::rbx: return "roblox";
            case category::none: return "erm_why";
        }

        return "unknown";
    }

    struct sig_name {
        category cat;
        std::string name;

        bool operator<(const sig_name &other) const;
    };

    extern const std::map<sig_name, sig_info> list;
}