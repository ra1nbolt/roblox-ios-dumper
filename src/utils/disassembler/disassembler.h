//
// Created by ra1nbolt on 5/12/26.
//

#pragma once

#include <capstone/capstone.h>
#include <capstone/arm64.h>
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include "../logger/logger.h"

namespace utils {
    class disassembler {
    private:
        csh handle;
        bool is_valid = false;

    public:
        disassembler();

        ~disassembler();

        bool status() const;

        size_t disassemble(const uint8_t *code, size_t size, uint64_t address, cs_insn **insn);

        void free_insn(cs_insn *insn, size_t count);

        uintptr_t resolve_adrp_add(const cs_insn *adrp, const cs_insn *add);
        uintptr_t resolve_bl(uintptr_t address);
        uintptr_t resolve_ldr(uintptr_t address, size_t depth = 10);
        uintptr_t resolve_adrl(uintptr_t address);

        bool writes_to_reg(const cs_insn *insn, arm64_reg reg);
    };
}