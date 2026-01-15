#pragma once
#include "../Utility/Include.h"
#include "CodeBook.h"
#include "../Utility/Singleton.h"
#include "../Parameters/Parameters.h"
#include "NR_CodeBook.h"
//@threads
#include "../SafeUnordered_map.h"
using namespace itpp;
using namespace std;

class CodeBookFactory : public singleton<CodeBookFactory> {
protected:
    // int TxPortNum
    std::unordered_map<int, std::shared_ptr<CodeBook> > m_NR_CodeBook_map;
    //@threads

public:
    //0for NR 1for LTE

    std::shared_ptr<CodeBook> GetCodeBook(int TxPortNum, const int _RATname = 0) {
        switch (_RATname) {
            case 1:
            case 0:
                return m_NR_CodeBook_map[TxPortNum];
            default:
                assert(false);
        }
        assert(false);
    }

public:
    CodeBookFactory();

    ~CodeBookFactory() = default;
protected:
    friend class singleton<CodeBookFactory>;
};

