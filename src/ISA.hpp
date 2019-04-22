#ifndef __ISA__HPP__
#define __ISA__HPP__

#include "asm.hpp"

class ISA {
public:
    struct AbstractInstruction {
        std::string name;
        std::string opcode;
        std::string funct;
    };

    AbstractInstruction absIns[64];
    std::map<std::string, size_t> instructionTable;
    std::set<std::string> insSet;
    std::map<std::string, int> registerNames;

    ISA(const std::string & isaFilename, const std::string & regFileName);
    bool validInstruction(const std::string & name) const;
    const std::string & getOPcode(const std::string & name) const;
    const std::string & getFunct(const std::string & name) const;
    int getRegNum(const std::string & name) const;
};

class Instruction {
private:
    std::string encodeInteger(int num, int bitwidth) const;

public:
    std::string instCode;

    Instruction();

    void clear();

    void setInstruction(const ISA & isa, const std::string & name);
    void setRsField(int rs);
    void setRtField(int rt);
    void setRdField(int rd);
    void setImm16(int imm16);
    void setImm26(int imm26);
    void setShamt(int shamt);

    void setWord(int wd);
};

#endif // !__ISA__HPP__
