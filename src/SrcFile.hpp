#ifndef __SRC__FILE__HPP__
#define __SRC__FILE__HPP__

#include "asm.hpp"

// (line number, content)
typedef std::pair<int, std::string> Line;

class SrcFile {
private:
    void eliminateComment();
    void eliminateComma();
    void eliminateSpace();
    void eliminateEmpty();
public:
    std::deque<Line> srcFile;

    SrcFile(const std::string & filename);

    void preProcess();
};

#endif // !__SRC__FILE__HPP__
