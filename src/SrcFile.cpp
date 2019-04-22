#include "asm.hpp"

SrcFile::SrcFile(const std::string & filename) {
    std::ifstream fin(filename);

    int lineNum = 0;
    std::string lineContent;
    while(std::getline(fin, lineContent)) {
        srcFile.emplace_back(std::make_pair(++lineNum, lineContent));
    }

    fin.close();
}

void SrcFile::eliminateComment() {
    for(auto it = srcFile.begin(); it != srcFile.end(); ++it) {
        std::string thisLine = it->second;
        size_t commentStart = thisLine.find('#');
        if(commentStart != std::string::npos) {
            thisLine.erase(commentStart);
        }
        it->second = thisLine;
    }
}

void SrcFile::eliminateComma() {
    for(auto it = srcFile.begin(); it != srcFile.end(); ++it) {
        std::string thisLine = it->second;
        for(auto & c : thisLine) {
            if(c == ',') c = ' ';
        }
        it->second = thisLine;
    }
}

void SrcFile::eliminateSpace() {
    for(auto it = srcFile.begin(); it != srcFile.end(); ++it) {
        std::string thisLine = it->second;
        size_t startPos = 0, endPos = thisLine.length() - 1;
        for(size_t i = 0; i < thisLine.length(); ++i) {
            if(!isspace(thisLine[i])) {
                startPos = i;
                break;
            }
        }
        for(size_t i = thisLine.length() - 1; i >= 0; --i) {
            if(!isspace(thisLine[i])) {
                endPos = i;
                break;
            }
        }
        thisLine.erase(endPos + 1);
        thisLine.erase(0, startPos);
        it->second = thisLine;
    }
}

void SrcFile::eliminateEmpty() {
    std::deque<Line> newSrcFile;
    for(auto it = srcFile.begin(); it != srcFile.end(); ++it) {
        if(it->second != "") {
            newSrcFile.push_back(*it);
        }
    }
    srcFile = newSrcFile;
}

void SrcFile::preProcess() {
    eliminateComment();
    eliminateComma();
    eliminateSpace();
    eliminateEmpty();
}

// int main() {
//     SrcFile src("sample.s");
//     src.preProcess();
//     for(auto line : src.srcFile) {
//         std::cout << line.first << ' ' << line.second << std::endl;
//     }
// }
