//
// Created by 31048 on 2023/1/11.
//

#include "../Parameters/Parameters.h"
#include "../CalcRENumperRB.h"

void  CalcRENum::GetRRCConfig(int SymbolperSH, int DMRSType, int DMRSSymbol, int OHperPRB){
    N_SymbolperSH = SymbolperSH;
    N_DMRSType = DMRSType;
    N_DMRSSymbol = DMRSSymbol;
    N_OHperPRB = OHperPRB;
    N_SCperRB = 12;
    //计算N_DMRSperRB
    GetDMRSperRB();
}

void  CalcRENum::GetDMRSperRB(){
    switch(N_DMRSSymbol){
        case DMRS_SingleSymbol:
            if(N_DMRSType == 1){
//                N_DMRSperRB = 12 / 2;
                N_DMRSperRB = 12;
            }else if(N_DMRSType == 2){
//                N_DMRSperRB = 12 / 3;
                N_DMRSperRB = 12;
            }else{
                assert(false);
            }
            break;
        case DMRS_DoubleSymbol:
            if(N_DMRSType == 1){
//                N_DMRSperRB = 24 / 2;
                N_DMRSperRB = 24;
            }else if(N_DMRSType == 2){
//                N_DMRSperRB = 24 / 3;
                N_DMRSperRB = 24;
            }else{
                assert(false);
            }
            break;
        default:
            assert(false);
            break;
    }
}

int CalcRENum::CalculateRENum(bool IsConfig){

    // 如果RRC层的信令没有变化，则直接返回旧有的N_RE
    if(IsConfig == 0 && N_RE != 0){
        return N_RE;
    }

    assert(N_OHperPRB == 0 || N_OHperPRB == 6 || N_OHperPRB == 12 ||  N_OHperPRB == 18 );
    N_RE = N_SCperRB * N_SymbolperSH - N_DMRSperRB - N_OHperPRB;
    //根据38.214中TBSize确定时，一个PRB最多有156个RE进行传输
    N_RE = 156 > N_RE ? N_RE : 156;
    assert(N_RE > 0 && N_RE <= 156);
//    int b = N_RE;
//    cout << b;
}

const CalcRENum& CalcRENum::Instance() {
    static CalcRENum& m_CalcRENum = *(new CalcRENum);
    return m_CalcRENum;
}

CalcRENum::CalcRENum(){

//    由于未实现RRC层，暂时使用Paramters里的参数
    int temp_SymbolperSH = 14;
    GetRRCConfig(temp_SymbolperSH, Parameters::Instance().OVERHEAD.DMRS_Type, Parameters::Instance().OVERHEAD.DMRS_Symbol, Parameters::Instance().OVERHEAD.OH_PRB);

}
