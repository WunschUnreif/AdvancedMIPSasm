#include "asm.hpp"

ISA::ISA(const std::string & isaFilename, const std::string & regFileName) {
    std::ifstream fin(isaFilename);
    
    int iCnt = 0;

    std::string name, opcode, funct;
    while(fin >> name >> opcode >> funct) {
        absIns[iCnt].name = name;
        absIns[iCnt].opcode = opcode;
        absIns[iCnt].funct = funct;

        insSet.insert(name);
        instructionTable[name] = iCnt++;
    }

    fin.close();

    fin.open(regFileName);

    int regNum = 0;
    std::string line;
    while(std::getline(fin, line)) {
        std::stringstream ss(line);
        std::string regName;
        while(ss >> regName) {
            registerNames[regName] = regNum;
        }
        ++regNum;
    }

    fin.close();
}

bool ISA::validInstruction(const std::string & name) const {
    return insSet.find(name) == insSet.end() ? false : true;
}

const std::string & ISA::getOPcode(const std::string & name) const {
    if(!validInstruction(name)) {
        throw std::runtime_error("[ISA]: Invalid instruction name!");
    }
    return absIns[instructionTable.at(name)].opcode;
}

const std::string & ISA::getFunct(const std::string & name) const {
    if(!validInstruction(name)) {
        throw std::runtime_error("[ISA]: Invalid instruction name!");
    }
    return absIns[instructionTable.at(name)].funct;
}

Instruction::Instruction() {
    //          0123456789*123456789*123456789*1
    //          | op | rs | rt | rd | sh | fnc |
    instCode = "00000000000000000000000000000000";
}

std::string Instruction::encodeInteger(int num, int bitwidth) const {
    std::string answer;

    for(int i = 32 - bitwidth; i < 32; ++i) {
        int thisBit = (num & (0x80000000U >> i)) == 0 ? 0 : 1;
        if(thisBit) {
            answer.append("1");
        } else {
            answer.append("0");
        }
    }

    return answer;
}

void Instruction::setInstruction(const ISA & isa, const std::string & name) {
    instCode.replace(0, 6, isa.getOPcode(name));
    instCode.replace(26, 6, isa.getFunct(name));
}

void Instruction::setRsField(int rs) {
    instCode.replace(6, 5, encodeInteger(rs, 5));
}

void Instruction::setRtField(int rt) {
    instCode.replace(11, 5, encodeInteger(rt, 5));
}

void Instruction::setRdField(int rd) {
    instCode.replace(16, 5, encodeInteger(rd, 5));
}

void Instruction::setImm16(int imm16) {
    instCode.replace(16, 16, encodeInteger(imm16, 16));
}

void Instruction::setImm26(int imm26) {
    instCode.replace(6, 26, encodeInteger(imm26, 26));
}

void Instruction::setShamt(int shamt) {
    instCode.replace(21, 5, encodeInteger(shamt, 5));
}

void Instruction::clear() {
    instCode = "00000000000000000000000000000000";
}

void Instruction::setWord(int wd) {
    instCode = encodeInteger(wd, 32);
}

int ISA::getRegNum(const std::string & name) const {
    if(registerNames.find(name) != registerNames.end()) {
        return registerNames.at(name);
    } else {
        return -1;
    }
}

// int main() {
//     ISA isa("ISA.def", "REG.def");
//     Instruction inst;
//     inst.setInstruction(isa, "addi");
//     inst.setRsField(10);
//     inst.setRtField(12);
//     inst.setRdField(31);
//     std::cout << inst.instCode << std::endl;
//     inst.clear();
//     inst.setWord(0xffffffff);
//     std::cout << inst.instCode << std::endl;

//     std::cout << isa.getRegNum("fp") << std::endl;
//     std::cout << isa.getRegNum("r29") << std::endl;
// }
