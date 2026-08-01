//
// Created by ra1nbolt on 5/20/26.
//

#include "signatures.h"

namespace signatures {
    bool sig_name::operator<(const sig_name &other) const {
        if (cat != other.cat) {
            return cat < other.cat;
        }

        return name < other.name;
    }

    const std::map<sig_name, sig_info> list = {
            {{category::lua, "luau_load"},           {"? ? ? 94 F5 03 00 AA ? ? ? 14 ? ? ? 38",                                                                                              resolve_type::bl}},
            {{category::lua, "luau_execute"},        {"? ? ? 94 ? ? ? 37 ? ? ? 39 ? ? ? A9",                                                                                                 resolve_type::bl}},
            {{category::lua, "lua_type"},            {"? ? ? A9 FD 03 00 91 ? ? ? 71 ? ? ? 54 ? ? ? A9 08 51 21 8B ? ? ? D1 1F 00 09 EB",                                                    resolve_type::none}},
            //{{category::lua, "lua_typename"},        {"? ? ? 31 ? ? ? 54 ? ? ? ? ? ? ? 91 ? ? ? F8 C0 03 5F D6",                                                                             resolve_type::none}},
            {{category::lua, "luaL_typename"},       {"? ? ? 94 ? ? ? 51 ? ? ? A9 ? ? ? A9",                                                                                                 resolve_type::bl}},
            {{category::lua, "lua_newthread"},       {"? ? ? 94 ? ? ? F8 ? ? ? D1 E1 03 14 AA ? ? ? 52",                                                                                     resolve_type::bl}},
            {{category::lua, "luaL_sandboxthread"},  {"? ? ? A9 ? ? ? A9 ? ? ? 91 F3 03 00 AA ? ? ? 52 ? ? ? 52 ? ? ? 94 E0 03 13 AA ? ? ? 52 ? ? ? 52 ? ? ? 94 E0 03 13 AA ? ? ? 12",       resolve_type::none}},
            {{category::lua, "lua_gettop"},          {"? ? ? 95 1B 00 14 4B",                                                                                                                resolve_type::bl}},
            {{category::lua, "lua_settop"},          {"? ? ? A9 ? ? ? A9 ? ? ? 91 F4 03 01 AA F3 03 00 AA ? ? ? 37 ? ? ? 90",                                                                resolve_type::none}},
            {{category::lua, "lua_createtable"},     {"? ? ? 94 E8 03 17 2A ? ? ? F9 39 19 08 8B",                                                                                           resolve_type::bl}},
            {{category::lua, "lua_getfield"},        {"? ? ? 94 ? ? ? 51 E0 03 15 AA ? ? ? 94",                                                                                              resolve_type::bl}},
            {{category::lua, "lua_setfield"},        {"? ? ? 94 E8 03 16 AA E0 03 08 AA",                                                                                                    resolve_type::bl}},
            {{category::lua, "lua_tolstring"},       {"? ? ? 97 F4 03 00 AA E2 03 00 91",                                                                                                    resolve_type::bl}},
            {{category::lua, "lua_pushlstring"},     {"? ? ? 97 68 03 18 0B",                                                                                                                resolve_type::bl}},
            {{category::lua, "lua_pushinteger"},     {"? ? ? 97 21 03 14 4B",                                                                                                                resolve_type::bl}},
            {{category::lua, "lua_pushnumber"},      {"? ? ? F9 ? ? ? FD ? ? ? 52 ? ? ? B9 ? ? ? F9",                                                                                        resolve_type::none}},
            {{category::lua, "lua_pushboolean"},     {"? ? ? F9 ? ? ? 71 E9 07 9F 1A ? ? ? B9 ? ? ? 52",                                                                                     resolve_type::none}},
            {{category::lua, "lua_pushcclosurek"},   {"? ? ? A9 ? ? ? A9 ? ? ? A9 ? ? ? A9 ? ? ? 91 F5 03 04 AA F4 03 03 AA F6 03 02 AA F7 03 01 AA F3 03 00 AA ? ? ? F9 ? ? ? A9",          resolve_type::none}},
            {{category::lua, "lua_pushnil"},         {"? ? ? F9 ? ? ? B9 ? ? ? 91 ? ? ? F9 C0 03 5F D6",                                                                                     resolve_type::none}},
            {{category::lua, "lua_pushvalue"},       {"? ? ? 94 E0 03 13 AA ? ? ? 94 96 7E 40 93",                                                                                           resolve_type::bl}},
            {{category::lua, "lua_call"},            {"? ? ? A9 ? ? ? A9 ? ? ? A9 ? ? ? 91 F4 03 02 AA F5 03 01 AA F3 03 00 AA ? ? ? 11",                                                    resolve_type::none}},
            {{category::lua, "luaL_error"},          {"? ? ? D1 ? ? ? A9 ? ? ? A9 ? ? ? 91 F3 03 01 AA F4 03 00 AA ? ? ? 91 ? ? ? F9 ? ? ? 52",                                              resolve_type::none}},
            {{category::lua, "luaL_typeerror"},      {"? ? ? D1 ? ? ? A9 ? ? ? A9 ? ? ? A9 ? ? ? 91 F5 03 02 AA F4 03 01 AA F3 03 00 AA ? ? ? 97 F6 03 00 AA",                               resolve_type::none}},
            {{category::lua, "luaL_argerror"},       {"? ? ? D1 ? ? ? A9 ? ? ? A9 ? ? ? A9 ? ? ? 91 F4 03 02 AA F5 03 01 AA F3 03 00 AA ? ? ? 94 ? ? ? B5 ? ? ? A9",                         resolve_type::none}},
            {{category::lua, "lua_ref"},             {"? ? ? A9 ? ? ? A9 ? ? ? A9 ? ? ? A9 ? ? ? 91 F3 03 00 AA ? ? ? F9 ? ? ? 71 ? ? ? 54 ? ? ? A9",                                        resolve_type::none}},
            {{category::lua, "lua_unref"},           {"? ? ? 71 ? ? ? 54 ? ? ? A9 ? ? ? A9 ? ? ? 91 F3 03 01 AA ? ? ? F9 ? ? ? F9",                                                          resolve_type::none}},
            {{category::lua, "lua_getmetatable"},    {"? ? ? 97 ? ? ? 34 E0 03 13 AA ? ? ? 12 ? ? ? 52",                                                                                     resolve_type::bl}},
            {{category::lua, "lua_setmetatable"},    {"? ? ? 94 E0 03 14 AA ? ? ? 12 ? ? ? 94 E0 03 14 AA ? ? ? 94 F4 03 00 AA",                                                             resolve_type::bl}},
            {{category::lua, "pseudo2addr"},         {"? ? ? 12 3F 00 08 6B ? ? ? 54 ? ? ? 12 3F 00 08 6B ? ? ? 54 ? ? ? 12",                                                                resolve_type::none}},
            {{category::lua, "luaO_nilobject"},      {"? ? ? ? ? ? ? 91 3F 01 0A 6B 60 C1 88 9A",                                                                                            resolve_type::adrl}},


            {{category::rbx, "pushinstance"},        {"? ? ? A9 ? ? ? A9 ? ? ? 91 F3 03 01 AA F4 03 00 AA ? ? ? 97 ? ? ? 34 E0 03 14 AA E1 03 13 AA ? ? ? A9 ? ? ? A8 ? ? ? 14",             resolve_type::none}},
            {{category::rbx, "get_global_state"},    {"? ? ? 97 F8 03 00 AA ? ? ? 39 ? ? ? 39",                                                                                              resolve_type::bl}},
            {{category::rbx, "get_extra_space"},     {"? ? ? 94 F9 03 00 AA ? ? ? 14 ? ? ? 12",                                                                                              resolve_type::bl}},
            {{category::rbx, "get_scriptcontext"},   {"? ? ? 94 ? ? ? 94 ? ? ? 91 E1 03 00 91",                                                                                              resolve_type::bl}},
            {{category::rbx, "spawn"},               {"? ? ? D1 ? ? ? A9 ? ? ? A9 ? ? ? A9 ? ? ? 91 F3 03 00 AA ? ? ? ? ? ? ? 91 ? ? ? 39 ? ? ? 37 ? ? ? 94",                                resolve_type::none}},
            {{category::rbx, "print"},               {"? ? ? D1 ? ? ? A9 ? ? ? A9 ? ? ? 91 E9 03 01 AA F3 03 00 AA ? ? ? 91",                                                                resolve_type::none}},
            {{category::rbx, "fireproximityprompt"}, {"? ? ? 97 ? ? ? 52 ? ? ? B9 ? ? ? 14 ? ? ? D2",                                                                                        resolve_type::bl}},
            {{category::rbx, "firetouchinterest"},   {"? ? ? A9 ? ? ? A9 ? ? ? A9 ? ? ? A9 ? ? ? 91 ? ? ? F9 ? ? ? B4 F3 03 04 AA F4 03 03 AA F5 03 02 AA F6 03 01 AA F7 03 00 AA ? ? ? F9", resolve_type::none}},
            {{category::rbx, "firemouseclick"},      {"? ? ? 97 ? ? ? 39 ? ? ? B4 ? ? ? A9 E1 03 00 91",                                                                                     resolve_type::bl}},
            {{category::rbx, "firerightmouseclick"}, {"? ? ? 97 ? ? ? 52 ? ? ? 14 ? ? ? 52 ? ? ? 14 ? ? ? 39",                                                                               resolve_type::bl}},

            {{category::rbx, "ktable"},              {"? ? ? ? ? ? ? 91 ? ? ? F8 E0 03 13 AA E1 03 15 AA",                                                                                   resolve_type::adrl}},
            {{category::rbx, "taskscheduler"},       {"? ? ? F9 E8 03 18 AA E1 03 17 AA",                                                                                                    resolve_type::ldr}},
            {{category::rbx, "opcode_lookup_table"}, {"? ? ? ? ? ? ? 91 ? ? ? 39 ? ? ? 38 ? ? ? 39 ? ? ? 38",                                                                                resolve_type::adrl}},



            {{category::none, "find_jobs"}, {"? ? ? 94 ? ? ? A9 3F 03 1B EB", resolve_type::bl}},
            {{category::none, "datamodeljob"}, {"? ? ? D1 ? ? ? A9 ? ? ? A9 ? ? ? A9 ? ? ? A9 ? ? ? A9 ? ? ? A9 ? ? ? 91 F7 03 03 AA F9 03 02 AA F8 03 01 AA", resolve_type::none}},
            {{category::none, "get_root_ancestor"}, {"? ? ? 94 F5 03 00 AA ? ? ? B4 ? ? ? 37", resolve_type::bl}},
            {{category::none, "scriptcontext_resume"}, {"? ? ? D1 ? ? ? 6D ? ? ? A9 ? ? ? A9 ? ? ? A9 ? ? ? A9 ? ? ? A9 ? ? ? A9 ? ? ? 91 F8 03 04 AA F9 03 03 AA FA 03 02 AA F5 03 01 AA", resolve_type::none}}
    };
}