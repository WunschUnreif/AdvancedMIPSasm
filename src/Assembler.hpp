#ifndef __ASSEMBLER__HPP__
#define __ASSEMBLER__HPP__

#include "asm.hpp"

#define CODE_SEG_SIZE (1024U)
#define DATA_SEG_SIZE (1024U)

// label, addr
extern std::map<std::string, int> labelTable;

class CodeAssembler {
private: 
    std::string eliminateSpace(std::string str);

    int readRegister(const std::string & rSrt, int line);
    int readImmediate(const std::string & iStr, int line);

    void psAddr();
    
    void i_Rtype(const std::string & content, int line);
    void i_Itype(const std::string & content, int line);
    void i_Jtype(const std::string & content, int line);
    void i_SHtype(const std::string & content, int line);
    void i_beq(const std::string & content, int line);
    void i_jr(const std::string & content, int line);
    void i_push(const std::string & content, int line);
    void i_pop(const std::string & content, int line);

    void processPseudo();
    void processLabel();
    void processInstruction();
    void processInstruction(const std::string & inst, int line);

    std::map<std::string, void (CodeAssembler::*)(const std::string &, int)> instTable;

public: 
    struct PendingInstruction {
        std::string instContent;
        int addr;
        int line;
    };

    ISA isa;
    std::deque<Instruction> codeMem;
    std::deque<std::string> codeOrig;
    SrcFile & src;
    std::deque<PendingInstruction> pendingList;
    int currAddr;

    CodeAssembler(SrcFile & srcfile);

    void start();
    void again();
    void dump(const std::string & filename);
};

class DataAssembler {
private:
    std::string eliminateSpace(std::string str); //

    int readImmediate(const std::string & iStr, int line); //

    void psWord(const std::string & content);
    void psAddr(const std::string & content);

    void processPseudo();   //
    void processLabel();    //

public:
    struct PendingInstruction {
        std::string instContent;
        int addr;
    };

    ISA isa;
    std::deque<Instruction> dataMem;
    SrcFile & src;
    std::deque<PendingInstruction> pendingList;
    int currAddr;

    DataAssembler(SrcFile & srcfile);   //

    void start();   //
    void again();   //
    void dump(const std::string & filename);    //
};

#endif // !__ASSEMBLER__HPP__
