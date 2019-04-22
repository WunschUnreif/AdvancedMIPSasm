#include "asm.hpp"

std::map<std::string, int> labelTable;

std::string comEliminateSpace(std::string str) {
    size_t startPos = 0, endPos = str.length() - 1;
    for(size_t i = 0; i < str.length(); ++i) {
        if(!isspace(str[i])) {
            startPos = i;
            break;
        }
    }
    for(size_t i = str.length() - 1; i >= 0; --i) {
        if(!isspace(str[i])) {
            endPos = i;
            break;
        }
    }
    str.erase(endPos + 1);
    str.erase(0, startPos);
    return str;
}

bool comReadRegister(const std::string rStr, int & num, ISA & isa) {
    num = isa.getRegNum(rStr);
    if(num == -1) return false;
    return true;
}

bool comReadImmediate(const std::string iStr, int & num) {
    if(iStr[0] == '0' && iStr[1] == 'x') {
        num = std::stoi(iStr.substr(2), 0, 16);
    } else {
        num = std::stoi(iStr);
    }
    return true;
}

std::string DataAssembler::eliminateSpace(std::string str) {
    return comEliminateSpace(str);
}

std::string CodeAssembler::eliminateSpace(std::string str) {
    return comEliminateSpace(str);
}

int CodeAssembler::readImmediate(const std::string & iStr, int line) {
    int numRead;
    if(!comReadImmediate(iStr, numRead)) {
        throw std::runtime_error("[CAS] Line " + std::to_string(line) + ": expected an immediate.");
    }
    return numRead;
}

int CodeAssembler::readRegister(const std::string & rStr, int line) {
    int numRead;
    if(!comReadRegister(rStr, numRead, isa)) {
        std::cerr << rStr << std::endl;
        throw std::runtime_error("[CAS] Line " + std::to_string(line) + ": expected a register.");
    }
    return numRead;
}

int DataAssembler::readImmediate(const std::string & iStr, int line) {
    int numRead;
    if(!comReadImmediate(iStr, numRead)) {
        throw std::runtime_error("[DAS] Line " + std::to_string(line) + ": expected an immediate.");
    }
    return numRead;
}

CodeAssembler::CodeAssembler(SrcFile & srcFile): 
    src(srcFile), 
    isa("ISA.def", "REG.def"),
    codeMem(CODE_SEG_SIZE),
    codeOrig(CODE_SEG_SIZE) {
    
    instTable["add"] = &CodeAssembler::i_Rtype;
    instTable["sub"] = &CodeAssembler::i_Rtype;
    instTable["and"] = &CodeAssembler::i_Rtype;
    instTable["or"] = &CodeAssembler::i_Rtype;
    instTable["slt"] = &CodeAssembler::i_Rtype;

    instTable["addi"] = &CodeAssembler::i_Itype;
    instTable["andi"] = &CodeAssembler::i_Itype;
    instTable["ori"] = &CodeAssembler::i_Itype;
    instTable["lw"] = &CodeAssembler::i_Itype;
    instTable["sw"] = &CodeAssembler::i_Itype;

    instTable["beq"] = &CodeAssembler::i_beq;
    instTable["jmp"] = &CodeAssembler::i_Jtype;
    instTable["jal"] = &CodeAssembler::i_Jtype;
    instTable["jr"] = &CodeAssembler::i_jr;

    instTable["sll"] = &CodeAssembler::i_SHtype;
    instTable["srl"] = &CodeAssembler::i_SHtype;

    instTable["push"] = &CodeAssembler::i_push;
    instTable["pop"] = &CodeAssembler::i_pop;

    currAddr = 0;
}

DataAssembler::DataAssembler(SrcFile & srcFile): 
    src(srcFile), 
    isa("ISA.def", "REG.def"),
    dataMem(DATA_SEG_SIZE) {
    
    currAddr = 0x00001000;
}

void CodeAssembler::again() {
    auto pending = pendingList;
    pendingList.clear();
    for(auto instruction : pending) {
        currAddr = instruction.addr;
        processInstruction(instruction.instContent, instruction.line);
    }
}

void DataAssembler::again() {
    auto pending = pendingList;
    pendingList.clear();
    for(auto instruction : pending) {
        currAddr = instruction.addr;
        psWord(instruction.instContent);
    }
}

void CodeAssembler::dump(const std::string & filename) {
    if(!pendingList.empty()) {
        throw std::runtime_error("[CAS] Some label cannot be found.");
    }

    std::ofstream fout(filename);

    for(int i = 0; i < CODE_SEG_SIZE; ++i) {
        fout << codeMem[i].instCode << "  // " << codeOrig[i] << std::endl;
    }

    fout.close();
}

void DataAssembler::dump(const std::string & filename) {
    if(!pendingList.empty()) {
        throw std::runtime_error("[CAS] Some label cannot be found.");
    }

    std::ofstream fout(filename);

    for(int i = 0; i < DATA_SEG_SIZE; ++i) {
        fout << dataMem[i].instCode << std::endl;
    }

    fout.close();
}

void CodeAssembler::start() {
    while(!src.srcFile.empty()) {
        std::string first;
        Line currLine = src.srcFile.front();
        std::stringstream ss(currLine.second);
        ss >> first;
        if(first == ".ends") {
            src.srcFile.pop_front();
            return;
        } else if(first[first.length() - 1] == ':') {
            processLabel();
        } else if(first[0] == '.') {
            processPseudo();
        } else {
            processInstruction();
        }
    }
}

void DataAssembler::start() {
    while(!src.srcFile.empty()) {
        std::string first;
        Line currLine = src.srcFile.front();
        std::stringstream ss(currLine.second);
        ss >> first;
        if(first == ".ends") {
            src.srcFile.pop_front();
            return;
        } else if(first[first.length() - 1] == ':') {
            processLabel();
        } else if(first[0] == '.') {
            processPseudo();
        } else {
            throw std::runtime_error("[DAS] unrecognized operation");
        }
    }
}

void CodeAssembler::processPseudo() {
    Line thisLine = src.srcFile.front();
    std::string content = thisLine.second;
    std::stringstream ss(content);

    std::string pseudo;
    ss >> pseudo;

    if(pseudo == ".addr") {
        psAddr();
    } else {
        throw std::runtime_error("[CAS] Line " + std::to_string(thisLine.first) + ": unrecognized option.");
    }
}

void CodeAssembler::processLabel() {
    Line thisLine = src.srcFile.front();
    std::string content = thisLine.second;
    std::stringstream ss(content);

    src.srcFile.pop_front();

    std::string label;
    ss >> label;

    if(content.length() > label.length()) {
        content = eliminateSpace(content.substr(label.length()));
        src.srcFile.push_front(std::make_pair(thisLine.first, content));
    }

    label = label.substr(0, label.length() - 1);
    
    if(labelTable.find(label) != labelTable.end()) {
        std::cerr << label << std::endl;
        throw std::runtime_error("[CAS] Line " + std::to_string(thisLine.first) + ": label redefined.");
    }

    labelTable[label] = currAddr;
}

void CodeAssembler::processInstruction(const std::string & inst, int line) {
    std::stringstream ss(inst);

    std::string opName;
    ss >> opName;

    if(instTable.find(opName) != instTable.end()) {
        (this->*instTable[opName])(inst, line);
    } else {
        throw std::runtime_error("[CAS] Line " + std::to_string(line) + ": invalid operation.");
    }
}

void CodeAssembler::processInstruction() {
    Line thisLine = src.srcFile.front();
    std::string content = thisLine.second;
    std::stringstream ss(content);

    src.srcFile.pop_front();

    std::string opName;
    ss >> opName;

    if(instTable.find(opName) != instTable.end()) {
        (this->*instTable[opName])(content, thisLine.first);
    } else {
        throw std::runtime_error("[CAS] Line " + std::to_string(thisLine.first) + ": invalid operation.");
    }
}

void CodeAssembler::psAddr() {
    Line thisLine = src.srcFile.front();
    std::string content = thisLine.second;
    std::stringstream ss(content);

    src.srcFile.pop_front();

    std::string pseudo, option;
    ss >> pseudo >> option;

    int addr = readImmediate(option, thisLine.first);
    if(addr < 0 || addr % 4 != 0) {
        throw std::runtime_error("[CAS] Line " + std::to_string(thisLine.first) + ": invalid address.");
    }
    currAddr = addr;
}

void CodeAssembler::i_Rtype(const std::string & content, int line) {
    std::stringstream ss(content);

    std::string inst, rd, rs, rt;
    ss >> inst >> rd >> rs >> rt;

    Instruction result;
    result.setInstruction(isa, inst);
    result.setRdField(readRegister(rd, line));
    result.setRsField(readRegister(rs, line));
    result.setRtField(readRegister(rt, line));
    codeMem[currAddr / 4] = result;
    codeOrig[currAddr / 4] = content;

    currAddr += 4;
}

void CodeAssembler::i_Itype(const std::string & content, int line) {
    std::stringstream ss(content);

    std::string inst, imm16, rs, rt;
    ss >> inst >> rt >> rs >> imm16;

    Instruction result;
    result.setInstruction(isa, inst);
    result.setRsField(readRegister(rs, line));
    result.setRtField(readRegister(rt, line));
    if(!isalpha(imm16[0])) {
        result.setImm16(readImmediate(imm16, line));
    } else {
        if(labelTable.find(imm16) != labelTable.end()) {
            result.setImm16(labelTable[imm16]);
        } else {
            PendingInstruction pend;
            pend.instContent = content;
            pend.line = line;
            pend.addr = currAddr;
            pendingList.push_back(pend);
            currAddr += 4;
            return;
        }
    }
    codeMem[currAddr / 4] = result;
    codeOrig[currAddr / 4] = content;

    currAddr += 4;
}

void CodeAssembler::i_beq(const std::string & content, int line) {
    std::stringstream ss(content);

    std::string inst, imm16, rs, rt;
    ss >> inst >> rt >> rs >> imm16;

    Instruction result;
    result.setInstruction(isa, inst);
    result.setRsField(readRegister(rs, line));
    result.setRtField(readRegister(rt, line));
    if(!isalpha(imm16[0])) {
        result.setImm16(readImmediate(imm16, line) / 4);
    } else {
        if(labelTable.find(imm16) != labelTable.end()) {
            result.setImm16((labelTable[imm16] - (currAddr + 4)) / 4);
        } else {
            PendingInstruction pend;
            pend.instContent = content;
            pend.line = line;
            pend.addr = currAddr;
            pendingList.push_back(pend);
            currAddr += 4;
            return;
        }
    }
    codeMem[currAddr / 4] = result;
    codeOrig[currAddr / 4] = content;

    currAddr += 4;
}

void CodeAssembler::i_Jtype(const std::string & content, int line) {
    std::stringstream ss(content);

    std::string inst, imm26;
    ss >> inst >> imm26;

    Instruction result;
    result.setInstruction(isa, inst);
    if(!isalpha(imm26[0])) {
        result.setImm26(readImmediate(imm26, line) / 4);
    } else {
        if(labelTable.find(imm26) != labelTable.end()) {
            result.setImm26(labelTable[imm26] / 4);
        } else {
            PendingInstruction pend;
            pend.instContent = content;
            pend.line = line;
            pend.addr = currAddr;
            pendingList.push_back(pend);
            currAddr += 4;
            return;
        }
    }
    codeMem[currAddr / 4] = result;
    codeOrig[currAddr / 4] = content;

    currAddr += 4;
}

void CodeAssembler::i_SHtype(const std::string & content, int line) {
    std::stringstream ss(content);

    std::string inst, rd, shamt, rt;
    ss >> inst >> rd >> rt >> shamt;

    Instruction result;
    result.setInstruction(isa, inst);
    result.setRdField(readRegister(rd, line));
    result.setRtField(readRegister(rt, line));
    result.setShamt(readImmediate(shamt, line));
    codeMem[currAddr / 4] = result;
    codeOrig[currAddr / 4] = content;

    currAddr += 4;
}

void CodeAssembler::i_jr(const std::string & content, int line) {
    std::stringstream ss(content);

    std::string inst, rs;
    ss >> inst >> rs;

    Instruction result;
    result.setInstruction(isa, inst);
    result.setRsField(readRegister(rs, line));
    codeMem[currAddr / 4] = result;
    codeOrig[currAddr / 4] = content;

    currAddr += 4;
}

void CodeAssembler::i_push(const std::string & content, int line) {
    std::stringstream ss(content);

    std::string inst, rt;
    ss >> inst >> rt;

    Instruction result;
    result.setInstruction(isa, inst);
    result.setRtField(readRegister(rt, line));
    codeMem[currAddr / 4] = result;
    codeOrig[currAddr / 4] = content;

    currAddr += 4;
}

void CodeAssembler::i_pop(const std::string & content, int line) {
    std::stringstream ss(content);

    std::string inst, rd;
    ss >> inst >> rd;

    Instruction result;
    result.setInstruction(isa, inst);
    result.setRdField(readRegister(rd, line));
    codeMem[currAddr / 4] = result;
    codeOrig[currAddr / 4] = content;

    currAddr += 4;
}

void DataAssembler::processPseudo() {
    Line thisLine = src.srcFile.front();
    std::string content = thisLine.second;
    std::stringstream ss(content);

    src.srcFile.pop_front();

    std::string pseudo;
    ss >> pseudo;

    if(pseudo == ".addr") {
        psAddr(content);
    } else if(pseudo == ".word") {
        psWord(content);
    } else {
        throw std::runtime_error("[DAS] Line " + std::to_string(thisLine.first) + ": unrecognized option.");
    }
}

void DataAssembler::processLabel() {
    Line thisLine = src.srcFile.front();
    std::string content = thisLine.second;
    std::stringstream ss(content);

    src.srcFile.pop_front();

    std::string label;
    ss >> label;

    if(content.length() > label.length()) {
        content = eliminateSpace(content.substr(label.length()));
        src.srcFile.push_front(std::make_pair(thisLine.first, content));
    }

    label = label.substr(0, label.length() - 1);
    
    if(labelTable.find(label) != labelTable.end()) {
        throw std::runtime_error("[DAS] Line " + std::to_string(thisLine.first) + ": label redefined.");
    }

    labelTable[label] = currAddr;
}

void DataAssembler::psAddr(const std::string & content) {
    std::stringstream ss(content);

    std::string pseudo, option;
    ss >> pseudo >> option;

    int addr = readImmediate(option, -1);
    if(addr < 0 || addr % 4 != 0) {
        throw std::runtime_error("[DAS] Line " + std::to_string(-1) + ": invalid address.");
    }
    currAddr = addr;
}

void DataAssembler::psWord(const std::string & content) {
    std::stringstream ss(content);

    std::string pseudo, imm;
    ss >> pseudo >> imm;

    Instruction result;
    if(isnumber(imm[0])) {
        result.setWord(readImmediate(imm, -1));
    } else {
        if(labelTable.find(imm) != labelTable.end()) {
            result.setWord(labelTable[imm]);
        } else {
            PendingInstruction pend;
            pend.instContent = content;
            pend.addr = currAddr;
            pendingList.push_back(pend);
            currAddr += 4;
            return;
        }
    }
    dataMem[(currAddr & 0x00000FFF) / 4] = result;
    currAddr += 4;
}

int main(int argc, char ** argv) {
    if(argc != 3) {
        std::cout << "usage: ./mas file.s output_name" << std::endl;
        return 0;
    }
    std::string filename = argv[1];
    std::string output = argv[2];

    SrcFile src(filename);
    src.preProcess();

    CodeAssembler ca(src);
    DataAssembler da(src);

    Line thisLine;
    while(!src.srcFile.empty()) {
        thisLine = src.srcFile.front();
        src.srcFile.pop_front();
        std::stringstream ss(thisLine.second);

        std::string ps, name;
        ss >> ps;
        if(ps != ".section") {
            std::cout << "Expected .section " << ps << std::endl;
            return 0;
        }

        ss >> name;
        try {
            if(name == "code") {
                ca.start();
            } else if(name == "data") {
                da.start();
            } else {
                std::cout << "Invalid section name" << std::endl;
                return 0;
            }
        } catch (std::exception & e) {
            std::cerr << e.what() << std::endl;
            return 0;
        }
    }

    try {
        ca.again();
        da.again();

        ca.dump(output + ".code");
        da.dump(output + ".data");
    } catch (std::exception & e) {
        std::cerr << e.what() << std::endl;
        return 0;
    }

    return 0;
}
