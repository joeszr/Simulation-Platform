///@file functions.cpp
///@brief  DB值与线性值之间的转换和线性插值函数的定义
///@author wangsen

#include <vector>
#include <cassert>
#include "../NetworkDrive/Clock.h"
#include<random>
#include "functions.h"
#include "../ChannelModel/functions.h"
#include "Random.h"

using namespace std;


///@brief 线性插值算法的函数定义
///
///@param _v 输入的插值向量，向量的元素可以是double,mat和cmat类型
///@param _iSpace 输入插值向量的有效数据之间的间隔，相邻两个元素之间的间隔为1，例如：1、0、3...则1和3之间的_iSpace=2
///@param _iOffset 输入插值向量的偏移，即第一个有效元素的位置
///@return void,但是该函数可以改变输入插值向量的值，插值后的结果仍然保存在输入参数_v中
///@see    
///@note 

void Interpolate(vector<double>::iterator const _ITBegin, vector<double>::iterator const _ITEnd, int _iSpace, int _iOffset) {
	if (_iSpace > 1) {
		//assert(_iOffset >= _iSpace || 0 == _iOffset);//mark
		assert(_iOffset >= 0);
		int iSize = static_cast<int> (_ITEnd - _ITBegin);
		assert(_iSpace < iSize && _iOffset < iSize);
		///偏移量为0时的情况
		if (_iOffset == 0) {
			int j = 0;
			for (int i = _iSpace; i < iSize; i = (i + _iSpace)) {
				for (int k = 0; k < _iSpace; ++k) {
					double slope = *(_ITBegin + i) - *(_ITBegin + i - _iSpace);
					double temp = slope * (static_cast<double> (k) / _iSpace) + *(_ITBegin + i - _iSpace);
					*(_ITBegin + i - _iSpace + k) = temp;
				}
				///记录循环条件结束时变量i的值，以便进行外插时使用
				j = i;
			}
			///补充插值向量最后一个元素
			if (j == (iSize - 1)) {
				*(_ITBegin + j) = *(_ITBegin + iSize - 1);
			}///根据循环条件结束时保存的i值，从第(j+1)个元素进行外插
			else {
				for (int k = 0; k < (iSize - j); ++k) {
					double slope = *(_ITBegin + j) - *(_ITBegin + j - _iSpace);
					double temp = slope * (static_cast<double> (k) / _iSpace) + *(_ITBegin + j);
					*(_ITBegin + j + k) = temp;
				}
			}
		}///偏移量大于0时的情况，偏移量不可能小于0
		else if (_iOffset > 0) {
			int j = 0;
			///由于此时偏移量大于0，所以首先对第0个元素到第(_iOffset-1)个元素进行向左外插
			for (int k = _iOffset; k > 0; --k) {
				double slope = *(_ITBegin + _iOffset + _iSpace) - *(_ITBegin + _iOffset);
				double temp = slope * (-1 * static_cast<double> (k) / _iSpace) + *(_ITBegin + _iOffset);
				*(_ITBegin + _iOffset - k) = temp;
			}
			///对第_iOffset个元素开始进行内插
			for (int i = (_iSpace + _iOffset); i < iSize; i = (i + _iSpace)) {
				for (int k = 0; k < _iSpace; ++k) {
					double slope = *(_ITBegin + i) - *(_ITBegin + i - _iSpace);
					double temp = slope * (static_cast<double> (k) / _iSpace) + *(_ITBegin + i - _iSpace);
					*(_ITBegin + i - _iSpace + k) = temp;
				}
				///记录循环条件结束时变量i的值，以便进行外插时使用
				j = i;
			}
			///补充插值向量最后一个元素
			if (j == (iSize - 1)) {
				*(_ITBegin + j) = *(_ITBegin + iSize - 1);
			}///根据循环结束时保存的变量i的值，从第(j+1)个元素进行向右外插
			else {
				for (int k = 0; k < (iSize - j); ++k) {
					double slope = *(_ITBegin + j) - *(_ITBegin + j - _iSpace);
					double temp = slope * (static_cast<double> (k) / _iSpace) + *(_ITBegin + j);
					*(_ITBegin + j + k) = temp;
				}
			}
		}
	}
}

void Interpolate(vector<mat>::iterator const _ITBegin, vector<mat>::iterator const _ITEnd, int _iSpace, int _iOffset) {
	if (_iSpace > 1) {
		assert(0 <= _iOffset);
		int iSize = static_cast<int> (_ITEnd - _ITBegin);
		assert(_iSpace < iSize && _iOffset < iSize);
		///偏移量为0时的情况
		if (_iOffset == 0) {
			int j = 0;
			for (int i = _iSpace; i < iSize; i = (i + _iSpace)) {
				for (int k = 0; k < _iSpace; ++k) {
					mat slope = *(_ITBegin + i) - *(_ITBegin + i - _iSpace);
					mat temp = slope * (static_cast<double> (k) / _iSpace) + *(_ITBegin + i - _iSpace);
					*(_ITBegin + i - _iSpace + k) = temp;
				}
				///记录循环条件结束时变量i的值，以便进行外插时使用
				j = i;
			}
			///补充插值向量最后一个元素
			if (j == (iSize - 1)) {
				*(_ITBegin + j) = *(_ITBegin + iSize - 1);
			}///根据循环条件结束时保存的i值，从第(j+1)个元素进行外插
			else {
				for (int k = 0; k < (iSize - j); ++k) {
					mat slope = *(_ITBegin + j) - *(_ITBegin + j - _iSpace);
					mat temp = slope * (static_cast<double> (k) / _iSpace) + *(_ITBegin + j);
					*(_ITBegin + j + k) = temp;
				}
			}
		}///偏移量大于0时的情况，偏移量不可能小于0
		else if (_iOffset > 0) {
			int j = 0;
			///由于此时偏移量大于0，所以首先对第0个元素到第(_iOffset-1)个元素进行向左外插
			for (int k = _iOffset; k > 0; --k) {
				mat slope = *(_ITBegin + _iOffset + _iSpace) - *(_ITBegin + _iOffset);
				mat temp = slope * (-1 * static_cast<double> (k) / _iSpace) + *(_ITBegin + _iOffset);
				*(_ITBegin + _iOffset - k) = temp;
			}
			///对第_iOffset个元素开始进行内插
			for (int i = (_iSpace + _iOffset); i < iSize; i = (i + _iSpace)) {
				for (int k = 0; k < _iSpace; ++k) {
					mat slope = *(_ITBegin + i) - *(_ITBegin + i - _iSpace);
					mat temp = slope * (static_cast<double> (k) / _iSpace) + *(_ITBegin + i - _iSpace);
					*(_ITBegin + i - _iSpace + k) = temp;
				}
				///记录循环条件结束时变量i的值，以便进行外插时使用
				j = i;
			}
			///补充插值向量最后一个元素
			if (j == (iSize - 1)) {
				*(_ITBegin + j) = *(_ITBegin + iSize - 1);
			}///根据循环结束时保存的变量i的值，从第(j+1)个元素进行向右外插
			else {
				for (int k = 0; k < (iSize - j); ++k) {
					mat slope = *(_ITBegin + j) - *(_ITBegin + j - _iSpace);
					mat temp = slope * (static_cast<double> (k) / _iSpace) + *(_ITBegin + j);
					*(_ITBegin + j + k) = temp;
				}
			}
		}
	}
}

void Interpolate(vector<cmat>::iterator const _ITBegin, vector<cmat>::iterator const _ITEnd, int _iSpace, int _iOffset) {
    assert(_iOffset >= _iSpace || 0 == _iOffset);
    int iSize = static_cast<int> (_ITEnd - _ITBegin);
    assert(_iSpace < iSize && _iOffset < iSize);
    ///偏移量为0时的情况
    if (_iOffset == 0) {
        int j = 0;
        for (int i = _iSpace; i < iSize; i = (i + _iSpace)) {
            for (int k = 0; k < _iSpace; ++k) {
                cmat slope = *(_ITBegin + i) - *(_ITBegin + i - _iSpace);
                cmat temp = (static_cast<double> (k) / _iSpace) * slope + *(_ITBegin + i - _iSpace);
                *(_ITBegin + i - _iSpace + k) = temp;
            }
            ///记录循环条件结束时变量i的值，以便进行外插时使用
            j = i;
        }
        ///补充插值向量最后一个元素
        if (j == (iSize - 1)) {
            *(_ITBegin + j) = *(_ITBegin + iSize - 1);
        }///根据循环条件结束时保存的i值，从第(j+1)个元素进行外插
        else {
            for (int k = 0; k < (iSize - j); ++k) {
                cmat slope = *(_ITBegin + j) - *(_ITBegin + j - _iSpace);
                cmat temp = (static_cast<double> (k) / _iSpace) * slope + *(_ITBegin + j);
                *(_ITBegin + j + k) = temp;
            }
        }
    }///偏移量大于0时的情况，偏移量不可能小于0
    else if (_iOffset > 0) {
        int j = 0;
        ///由于此时偏移量大于0，所以首先对第0个元素到第(_iOffset-1)个元素进行向左外插
        for (int k = _iOffset; k > 0; --k) {
            cmat slope = *(_ITBegin + _iOffset + _iSpace) - *(_ITBegin + _iOffset);
            cmat temp = (-1 * static_cast<double> (k) / _iSpace) * slope + *(_ITBegin + _iOffset);
            *(_ITBegin + _iOffset - k) = temp;
        }
        ///对第_iOffset个元素开始进行内插
        for (int i = (_iSpace + _iOffset); i < iSize; i = (i + _iSpace)) {
            for (int k = 0; k < _iSpace; ++k) {
                cmat slope = *(_ITBegin + i) - *(_ITBegin + i - _iSpace);
                cmat temp = (static_cast<double> (k) / _iSpace) * slope + *(_ITBegin + i - _iSpace);
                *(_ITBegin + i - _iSpace + k) = temp;
            }
            ///记录循环条件结束时变量i的值，以便进行外插时使用
            j = i;
        }
        ///补充插值向量最后一个元素
        if (j == (iSize - 1)) {
            *(_ITBegin + j) = *(_ITBegin + iSize - 1);
        }///根据循环结束时保存的变量i的值，从第(j+1)个元素进行向右外插
        else {
            for (int k = 0; k < (iSize - j); ++k) {
                cmat slope = *(_ITBegin + j) - *(_ITBegin + j - _iSpace);
                cmat temp = (static_cast<double> (k) / _iSpace) * slope + *(_ITBegin + j);
                *(_ITBegin + j + k) = temp;
            }
        }
    }
}

void InterpolateLog(vector<double>& _v, int _iSpace, int _iOffset) {
    if(_iSpace>1){
        for (auto& d : _v) {
            d = L2DB(d);
        }
        Interpolate(_v, _iSpace, _iOffset);
        for (auto& d : _v) {
            d = DB2L(d);
        }
    }
}

void InterpolateLog(vector<mat>& _v, int _iSpace, int _iOffset) {
    for (auto& d : _v) {
        d = L2DB(d);
    }
    Interpolate(_v, _iSpace, _iOffset);
    for (auto& d : _v) {
        d = DB2L(d);
    }
}

void InterpolateLog(vector<double>::iterator const _ITBegin, vector<double>::iterator const _ITEnd, int _iSpace, int _iOffset) {
    int iSize = static_cast<int> (_ITEnd - _ITBegin);
    for (int i = 0; i < iSize; ++i) {
        *(_ITBegin + i) = L2DB(*(_ITBegin + i));
    }
    Interpolate(_ITBegin, _ITEnd, _iSpace, _iOffset);
    for (int i = 0; i < iSize; ++i) {
        *(_ITBegin + i) = DB2L(*(_ITBegin + i));
    }
}

void InterpolateLog(vector<mat>::iterator const _ITBegin, vector<mat>::iterator const _ITEnd, int _iSpace, int _iOffset) {
	if (_iSpace > 1) {
		int iSize = static_cast<int> (_ITEnd - _ITBegin);
		for (int i = 0; i < iSize; ++i) {
			*(_ITBegin + i) = L2DB(*(_ITBegin + i));
		}
		Interpolate(_ITBegin, _ITEnd, _iSpace, _iOffset);
		for (int i = 0; i < iSize; ++i) {
			*(_ITBegin + i) = DB2L(*(_ITBegin + i));
		}
	}
}

double xUniform_msconstruct(double _dmin, double _dmax) {
    Random random;  // 创建 Random 对象
    return random.xUniform_msconstruct(_dmin, _dmax);  // 通过对象调用非静态成员函数
}


///思考以下
double xUniform_channel(double _dmin, double _dmax) {
    return Random::xUniform_channel(_dmin, _dmax);
}
///
int xUniformInt(const int _imin, const int _imax) {

    return Random::xUniformInt(_imin, _imax);
}

double xUniform_distributems(double _dmin, double _dmax) {
    return Random::xUniform_distributems(_dmin, _dmax);
}
///20260119
double xUniform_distributepico(double _dmin, double _dmax) {
    Random random;
    return random.xUniform_distributepico(_dmin, _dmax);  // 修复：使用对象实例调用方法
}
double xNormal_channel(double _dave, double _dstd) {
    Random random;
    return random.xNormal_channel(_dave, _dstd);
}
double xNormal_msconstruct(double _dave, double _dstd) {
    Random random;
    return random.xNormal_msconstruct(_dave, _dstd);
}
/// 线性值到DB值的转换
double L2DB(double _dLinear) {
    return 10 * log10(_dLinear);
}

mat L2DB(mat _mLinear) {
    mat mL(_mLinear.rows(), _mLinear.cols());
    for (int ir = 0; ir < _mLinear.rows(); ++ir)
        for (int ic = 0; ic < _mLinear.cols(); ++ic)
            mL(ir, ic) = L2DB(_mLinear(ir, ic));
    return mL;
}

/// DB值到线性值的转换

double DB2L(double _dB) {
    return pow(10.0, _dB / 10.0);
}

mat DB2L(mat _mdB) {
    mat mL(_mdB.rows(), _mdB.cols());
    for (int ir = 0; ir < _mdB.rows(); ir++)
        for (int ic = 0; ic < _mdB.cols(); ic++)
            mL(ir, ic) = DB2L(_mdB(ir, ic));
    return mL;
}

int DownOrUpLink(int _iTime) {
    const int static DOWNLINK = 0;
    const int static UPLINK = 1;
    const int static Special = 2;
    const int static GP = -1;


    assert(Parameters::Instance().SIM.FrameStructure4Sim.P1.IDLSlot + Parameters::Instance().SIM.FrameStructure4Sim.P1.IULSlot + Parameters::Instance().SIM.FrameStructure4Sim.P1.ISpecialSlot
            == Parameters::Instance().SIM.FrameStructure4Sim.P1.IPeriodSlot);
    assert(Parameters::Instance().SIM.FrameStructure4Sim.P2.IDLSlot + Parameters::Instance().SIM.FrameStructure4Sim.P2.IULSlot + Parameters::Instance().SIM.FrameStructure4Sim.P2.ISpecialSlot
            == Parameters::Instance().SIM.FrameStructure4Sim.P2.IPeriodSlot);
   
    if(Parameters::Instance().SIM.FrameStructure4Sim.IIsSinglePeriod==1){
        int iSlot = ((_iTime - 1) % Parameters::Instance().SIM.FrameStructure4Sim.P1.IPeriodSlot) + 1;
        if (iSlot <= Parameters::Instance().SIM.FrameStructure4Sim.P1.IDLSlot) {
            return DOWNLINK;
        } else if (iSlot == Parameters::Instance().SIM.FrameStructure4Sim.P1.IDLSlot + Parameters::Instance().SIM.FrameStructure4Sim.P1.ISpecialSlot) {
            return Special;
        } else if (iSlot >= Parameters::Instance().SIM.FrameStructure4Sim.P1.IPeriodSlot
                - Parameters::Instance().SIM.FrameStructure4Sim.P1.IULSlot + 1) {
            return UPLINK;
        } else {
            return GP;
        }
    }
    else if(Parameters::Instance().SIM.FrameStructure4Sim.IIsSinglePeriod==0){
        int iSlot = ((_iTime - 1) % (Parameters::Instance().SIM.FrameStructure4Sim.P1.IPeriodSlot + Parameters::Instance().SIM.FrameStructure4Sim.P2.IPeriodSlot)) + 1;
        if (iSlot <= Parameters::Instance().SIM.FrameStructure4Sim.P1.IPeriodSlot) {
            if (iSlot <= Parameters::Instance().SIM.FrameStructure4Sim.P1.IDLSlot) {
                return DOWNLINK;
            } else if (iSlot == Parameters::Instance().SIM.FrameStructure4Sim.P1.IDLSlot + Parameters::Instance().SIM.FrameStructure4Sim.P1.ISpecialSlot) {
                return Special;
            } else if (iSlot >= Parameters::Instance().SIM.FrameStructure4Sim.P1.IPeriodSlot - Parameters::Instance().SIM.FrameStructure4Sim.P1.IULSlot + 1) {
                return UPLINK;
            } else {
                return GP;
            }
        }
        else {
            iSlot -= Parameters::Instance().SIM.FrameStructure4Sim.P1.IPeriodSlot;
            if (iSlot <= Parameters::Instance().SIM.FrameStructure4Sim.P2.IDLSlot) {
                return DOWNLINK;
            } else if (iSlot == Parameters::Instance().SIM.FrameStructure4Sim.P2.IDLSlot + Parameters::Instance().SIM.FrameStructure4Sim.P2.ISpecialSlot) {
                return Special;
            } else if (iSlot >= Parameters::Instance().SIM.FrameStructure4Sim.P2.IPeriodSlot
                    - Parameters::Instance().SIM.FrameStructure4Sim.P2.IULSlot + 1) {
                return UPLINK;
            } else {
                return GP;
            }
        }
    }
    else{
        assert(false);
    }
    assert(false);
    return  -1;
}
int GetULSlotNum(int Period)
{
    int ULSlotNum = 0;
    for(int j=1;j<=Period;j++)
    {
        if(DownOrUpLink(j) == 1)
            ULSlotNum++;
    }
    return ULSlotNum;
}

void BD(const pair<cmat, cmat>& _rChannelMat, pair<cmat, cmat>& _rCodeWord, pair<double, double>& _rSLR, pair<double, double>& _rBFGain) {
    itpp::cmat mH1 = _rChannelMat.first;
    itpp::cmat mH2 = _rChannelMat.second;

    int iRank1 = mH1.rows();
    int iRank2 = mH2.rows();

    itpp::cmat mU1, mD1, mU2, mD2;
    itpp::vec vS1, vS2;

    itpp::svd(mH1, mU1, vS1, mD1);
    itpp::svd(mH2, mU2, vS2, mD2);

    //计算Null Space
    itpp::cmat mN1 = mD1.get_cols(iRank1, mD1.cols() - 1);
    itpp::cmat mN2 = mD2.get_cols(iRank2, mD2.cols() - 1);

    itpp::cmat mK1 = mH1 * mN2;
    itpp::cmat mK2 = mH2 * mN1;

    itpp::cmat mU3, mD3, mU4, mD4;
    itpp::vec vS3, vS4;

    itpp::svd(mK1, mU3, vS3, mD3);
    itpp::svd(mK2, mU4, vS4, mD4);

    itpp::cmat mV1 = mD3.get_cols(0, 0);
    itpp::cmat mV2 = mD4.get_cols(0, 0);

    mV1 = mN2 * mV1;
    mV2 = mN1 * mV2;

    double dSLR1 = real(mV1.H() * mH1.H() * mH1 * mV1)(0, 0) / real(mV1.H() * mH2.H() * mH2 * mV1)(0, 0);
    double dSLR2 = real(mV2.H() * mH2.H() * mH2 * mV2)(0, 0) / real(mV2.H() * mH1.H() * mH1 * mV2)(0, 0);

    double dBFGain1 = pow(vS3(0), 2.0);
    double dBFGain2 = pow(vS4(0), 2.0);

    _rCodeWord = make_pair(mV1, mV2);
    _rSLR = make_pair(dSLR1, dSLR2);
    _rBFGain = make_pair(dBFGain1, dBFGain2);
}

void BDR(const pair<cmat, cmat>& _rCovMat, pair<cmat, cmat>& _rCodeWord, pair<double, double>& _rSLR, pair<double, double>& _rBFGain) {
    itpp::cmat mA1 = _rCovMat.first;
    itpp::cmat mA2 = _rCovMat.second;

    itpp::cmat mU1, mD1, mU2, mD2;
    itpp::vec vS1, vS2;

    itpp::svd(mA1, mU1, vS1, mD1);
    itpp::svd(mA2, mU2, vS2, mD2);

    //计算Null Space
    itpp::cmat mN1 = mD1.get_cols(mD1.cols() - 2, mD1.cols() - 1);
    itpp::cmat mN2 = mD2.get_cols(mD2.cols() - 2, mD2.cols() - 1);

    itpp::cmat mK1 = mN2.H() * mA1 * mN2;
    itpp::cmat mK2 = mN1.H() * mA2 * mN1;

    itpp::cmat mU3, mD3, mU4, mD4;
    itpp::vec vS3, vS4;

    itpp::svd(mK1, mU3, vS3, mD3);
    itpp::svd(mK2, mU4, vS4, mD4);

    itpp::cmat mV1 = mD3.get_cols(0, 0);
    itpp::cmat mV2 = mD4.get_cols(0, 0);

    mV1 = mN2 * mV1;
    mV2 = mN1 * mV2;

    double dSLR1 = real(mV1.H() * mA1 * mV1)(0, 0) / real(mV1.H() * mA2 * mV1)(0, 0);
    double dSLR2 = real(mV2.H() * mA2 * mV2)(0, 0) / real(mV2.H() * mA1 * mV2)(0, 0);

    double dBFGain1 = vS3(0) / vS1(0);
    double dBFGain2 = vS4(0) / vS2(0);

    _rCodeWord = make_pair(mV1, mV2);
    _rSLR = make_pair(dSLR1, dSLR2);
    _rBFGain = make_pair(dBFGain1, dBFGain2);
}

void BD(const vector<cmat>& _vChannelMat, vector<cmat>& _vCodeWord) {
    int iNum = static_cast<int> (_vChannelMat.size());
    for (int i = 0; i < iNum; ++i) {
        itpp::cmat mH_other;
        for (int j = 0; j < iNum; ++j) {
            if (i != j)
                mH_other = concat_vertical(mH_other, _vChannelMat[j]);
        }

        int iRank = mH_other.rows();

        itpp::cmat mU1, mD1;
        itpp::vec vS1;
        itpp::svd(mH_other, mU1, vS1, mD1);

        itpp::cmat mNull = mD1.get_cols(iRank, mD1.cols() - 1);
        itpp::cmat mH = _vChannelMat[i] * mNull;

        itpp::cmat mU2, mD2;
        itpp::vec vS2;
        itpp::svd(mH, mU2, vS2, mD2);

        itpp::cmat mV = mD2.get_cols(0, 0);
        mV = mNull * mV;

        _vCodeWord.push_back(mV);
    }
}

void BDR(const vector<cmat>& _vCovR, vector<cmat>& _vCodeWord) {
    int iNum = static_cast<int> (_vCovR.size());
    int iMSAntennaNum = Parameters::Instance().MSS.FirstBand.ITotalAntNumPerPanel;
    int iRow = _vCovR[0].rows();
    for (int i = 0; i < iNum; ++i) {
        itpp::cmat mCovR_other = zeros_c(iRow, iRow);
        for (int j = 0; j < iNum; ++j) {
            if (i != j)
                mCovR_other = mCovR_other + _vCovR[j];
        }

        int iRank_other = iMSAntennaNum * (iNum - 1);

        itpp::cmat mU1, mD1;
        itpp::vec vS1;
        itpp::svd(mCovR_other, mU1, vS1, mD1);

        itpp::cmat mNull = mD1.get_cols(iRank_other, mD1.cols() - 1);
        itpp::cmat mCovR = mNull.H() * _vCovR[i] * mNull;

        itpp::cmat mU2, mD2;
        itpp::vec vS2;
        itpp::svd(mCovR, mU2, vS2, mD2);

        itpp::cmat mV = mD2.get_cols(0, 0);
        mV = mNull * mV;

        _vCodeWord.push_back(mV);
    }
}

void BDR_RankA(
        const vector<cmat>& _vCovR,
        vector<cmat>& _vCodeWord,
        vector<int>& _vRank) {

    int iNum = static_cast<int> (_vCovR.size());
    //    int iMSAntennaNum = Parameters::Instance().MSS.FirstBand.ITotalAntNumPerPanel;
    int iRow = _vCovR[0].rows();

    /*
        cmat mPPortleft_UE = (ProductmPPortleft_UE(10, 10)).T(); //只想取UE的Port数目，参数10 任意
        int iUEPort = mPPortleft_UE.cols();
     */


    int H_TXRU_DIV_NUM_PerPanel = Parameters::Instance().MSS.FirstBand.H_TXRU_DIV_NUM_PerPanel;
    int V_TXRU_DIV_NUM_PerPanel = Parameters::Instance().MSS.FirstBand.V_TXRU_DIV_NUM_PerPanel;
    int Polarize_Num = Parameters::Instance().MSS.FirstBand.Polarize_Num;

    int iUEPort =
            H_TXRU_DIV_NUM_PerPanel * V_TXRU_DIV_NUM_PerPanel * Polarize_Num;
    for (int i = 0; i < iNum; ++i) {
        itpp::cmat mCovR_other = zeros_c(iRow, iRow);
        for (int j = 0; j < iNum; ++j) {
            if (i != j)
                mCovR_other = mCovR_other + _vCovR[j];
        }

        int iRank_other = iUEPort * (iNum - 1);

        itpp::cmat mU1, mD1;
        itpp::vec vS1;
        itpp::svd(mCovR_other, mU1, vS1, mD1);

        itpp::cmat mNull = mD1.get_cols(iRank_other, mD1.cols() - 1);
        itpp::cmat mCovR = mNull.H() * _vCovR[i] * mNull;

        itpp::cmat mU2, mD2;
        itpp::vec vS2;
        itpp::svd(mCovR, mU2, vS2, mD2);

        int iRank = _vRank[i];
        itpp::cmat mV = mD2.get_cols(0, iRank);
        mV = mNull * mV;

        _vCodeWord.push_back(mV);
    }
}

void BD_RankA(const vector<cmat>& _vChannelMat, vector<cmat>& _vCodeWord, vector<int>& _vRank) {
    int iNum = static_cast<int> (_vChannelMat.size());
    for (int i = 0; i < iNum; ++i) {
        itpp::cmat mH_other;
        for (int j = 0; j < iNum; ++j) {
            if (i != j)
                mH_other = concat_vertical(mH_other, _vChannelMat[j]);
        }

        int iRank_other = mH_other.rows();

        itpp::cmat mU1, mD1;
        itpp::vec vS1;
        itpp::svd(mH_other, mU1, vS1, mD1);

        itpp::cmat mNull = mD1.get_cols(iRank_other, mD1.cols() - 1);
        itpp::cmat mH = _vChannelMat[i] * mNull;

        itpp::cmat mU2, mD2;
        itpp::vec vS2;
        itpp::svd(mH, mU2, vS2, mD2);

        int iRank = _vRank[i];
        itpp::cmat mV = mD2.get_cols(0, iRank);
        mV = mNull * mV;

        _vCodeWord.push_back(mV);
    }
}

void MET(const pair<cmat, cmat>& _rChannelMat, pair<cmat, cmat>& _rCodeWord, pair<double, double>& _rSLR, pair<double, double>& _rBFGain) {
    itpp::cmat mH1_MET = _rChannelMat.first;
    itpp::cmat mH2_MET = _rChannelMat.second;

    itpp::cmat mU1_MET, mD1_MET, mU2_MET, mD2_MET;
    itpp::vec vS1_MET, vS2_MET;

    itpp::svd(mH1_MET, mU1_MET, vS1_MET, mD1_MET);
    itpp::svd(mH2_MET, mU2_MET, vS2_MET, mD2_MET);

    itpp::cmat mH1 = mU1_MET.H().get_rows(0, 0) * mH1_MET;
    itpp::cmat mH2 = mU2_MET.H().get_rows(0, 0) * mH2_MET;

    int iRank1 = mH1.rows();
    int iRank2 = mH2.rows();

    itpp::cmat mU1, mD1, mU2, mD2;
    itpp::vec vS1, vS2;

    itpp::svd(mH1, mU1, vS1, mD1);
    itpp::svd(mH2, mU2, vS2, mD2);

    //计算Null Space
    itpp::cmat mN1 = mD1.get_cols(iRank1, mD1.cols() - 1);
    itpp::cmat mN2 = mD2.get_cols(iRank2, mD2.cols() - 1);

    itpp::cmat mK1 = mH1 * mN2;
    itpp::cmat mK2 = mH2 * mN1;

    itpp::cmat mU3, mD3, mU4, mD4;
    itpp::vec vS3, vS4;

    itpp::svd(mK1, mU3, vS3, mD3);
    itpp::svd(mK2, mU4, vS4, mD4);

    itpp::cmat mV1 = mD3.get_cols(0, 0);
    itpp::cmat mV2 = mD4.get_cols(0, 0);

    mV1 = mN2 * mV1;
    mV2 = mN1 * mV2;

    //    double dSLR1 = real(mV1.H() * mH1.H() * mH1 * mV1)(0, 0) / real(mV1.H() * mH2.H() * mH2 * mV1)(0, 0);
    //    double dSLR2 = real(mV2.H() * mH2.H() * mH2 * mV2)(0, 0) / real(mV2.H() * mH1.H() * mH1 * mV2)(0, 0);

    double dSLR1 = real(mV1.H() * mH1.H() * mH1 * mV1)(0, 0) / real(mV2.H() * mH1.H() * mH1 * mV2)(0, 0);
    double dSLR2 = real(mV2.H() * mH2.H() * mH2 * mV2)(0, 0) / real(mV1.H() * mH2.H() * mH2 * mV1)(0, 0);

    double dBFGain1 = pow(vS3(0), 2.0);
    double dBFGain2 = pow(vS4(0), 2.0);

    _rCodeWord = make_pair(mV1, mV2);
    _rSLR = make_pair(dSLR1, dSLR2);
    _rBFGain = make_pair(dBFGain1, dBFGain2);
}
void MET_RankA(
        const vector<cmat>& _vH,
        vector<cmat>& _vCodeWord,
        vector<int> _vRank, bool _bIsCovR) {

    _vCodeWord.clear();

    int iUENum = static_cast<int> (_vH.size());
    int iRow = _vH[0].rows();
    int iCol = _vH[0].cols();

    for (int i = 0; i < iUENum; i++) {
        _vRank[i] += 1;
    }
    vector<cmat> vL1_METs;
    for (int i = 0; i < iUENum; ++i) {
        int iRank = _vRank[i];
        vL1_METs.push_back(itpp::zeros_c(iRank, iCol));
    }
    int TotalRank = 0;
    for (int i = 0; i < iUENum; ++i) {
        itpp::cmat mH1_MET = _vH[i];
        int iRank = _vRank[i];

        itpp::cmat mL1_MET;
        CalcL_for_MET_RankA(mH1_MET, iRank, mL1_MET, _bIsCovR);

        vL1_METs[i] = mL1_MET;

        TotalRank += iRank;
    }


    for (int i = 0; i < iUENum; ++i) {
        int iRank = _vRank[i];
        int iTotalRank_other = TotalRank - iRank;

        itpp::cmat mL1_MET;
        mL1_MET = vL1_METs[i];

        int StartingRankPosition = 0;

        itpp::cmat CombL_of_others = itpp::zeros_c(iTotalRank_other, iCol);
        for (int j = 0; j < iUENum; ++j) {
            if (j != i) {
                int iCurOtherRank = _vRank[j];
                itpp::cmat mLj_MET;
                mLj_MET = vL1_METs[j];

                CombL_of_others.set_submatrix(
                        StartingRankPosition,
                        StartingRankPosition + iCurOtherRank - 1,
                        0, iCol - 1, mLj_MET);

                StartingRankPosition += iCurOtherRank;
            }
        }

        itpp::cmat mU_others, mV_others;
        itpp::vec vS_others;
        itpp::svd(CombL_of_others, mU_others, vS_others, mV_others);

        itpp::cmat mNull = mV_others.get_cols(iTotalRank_other, mV_others.cols() - 1);


        itpp::cmat mG = mL1_MET * mNull;


        itpp::cmat mU_2, mV_2;
        itpp::vec vS_2;
        itpp::svd(mG, mU_2, vS_2, mV_2);

        itpp::cmat mV = mV_2.get_cols(0, iRank - 1);

        itpp::cmat mB = mNull * mV;

        _vCodeWord.push_back(mB);

    }

}

void MET_Rank0(
        const vector<cmat>& _vH,
        vector<cmat>& _vCodeWord, bool _bIsCovR) {

    vector<int> _vRank(_vCodeWord.size(), 0);
    MET_RankA(_vH, _vCodeWord, _vRank, _bIsCovR);
}

//用于AI反馈CSI
void MET_RankA(
        const vector<cmat>& _vV,vector<itpp::vec> _vS,
        vector<cmat>& _vCodeWord,
        vector<int> _vRank, bool _bIsCovR) {

    _vCodeWord.clear();

    int iUENum = static_cast<int> (_vV.size());
//    int iRow = _vH[0].rows();
    int iCol = _vV[0].rows();

    for (int i = 0; i < iUENum; i++) {
        _vRank[i] += 1;
    }
    vector<cmat> vL1_METs;
    for (int i = 0; i < iUENum; ++i) {
        int iRank = _vRank[i];
        vL1_METs.push_back(itpp::zeros_c(iRank, iCol));
    }
    int TotalRank = 0;
    for (int i = 0; i < iUENum; ++i) {
        itpp::cmat mV1_MET = _vV[i];
        itpp::vec S=_vS[i];
        int iRank = _vRank[i];

        itpp::cmat mL1_MET;
        CalcL_for_MET_RankA(mV1_MET, S, iRank, mL1_MET, _bIsCovR);

        vL1_METs[i] = mL1_MET;

        TotalRank += iRank;
    }


    for (int i = 0; i < iUENum; ++i) {
        int iRank = _vRank[i];
        int iTotalRank_other = TotalRank - iRank;

        itpp::cmat mL1_MET;
        mL1_MET = vL1_METs[i];

        int StartingRankPosition = 0;

        itpp::cmat CombL_of_others = itpp::zeros_c(iTotalRank_other, iCol);
        for (int j = 0; j < iUENum; ++j) {
            if (j != i) {
                int iCurOtherRank = _vRank[j];
                itpp::cmat mLj_MET;
                mLj_MET = vL1_METs[j];//

                CombL_of_others.set_submatrix(
                        StartingRankPosition,
                        StartingRankPosition + iCurOtherRank - 1,
                        0, iCol - 1, mLj_MET);

                StartingRankPosition += iCurOtherRank;
            }
        }

        itpp::cmat mU_others, mV_others;
        itpp::vec vS_others;
        itpp::svd(CombL_of_others, mU_others, vS_others, mV_others);

        itpp::cmat mNull = mV_others.get_cols(iTotalRank_other, mV_others.cols() - 1);


        itpp::cmat mG = mL1_MET * mNull;


        itpp::cmat mU_2, mV_2;
        itpp::vec vS_2;
        itpp::svd(mG, mU_2, vS_2, mV_2);

        itpp::cmat mV = mV_2.get_cols(0, iRank - 1);

        itpp::cmat mB = mNull * mV;

        _vCodeWord.push_back(mB);

    }

}
void CalcL_for_MET_RankA(
        itpp::cmat& mV1_MET, itpp::vec vS1_MET ,int iRank, itpp::cmat& _mL1_MET, bool _bIsCovR) {

    // rank start from 1
//    itpp::cmat mU1_MET, mV1_MET;
//    itpp::vec vS1_MET;
//    itpp::svd(mH1_MET, mU1_MET, vS1_MET, mV1_MET);

    itpp::mat mRank_S1 = zeros(iRank, iRank);
    for (int i = 0; i < iRank; i++) {
        double lamda = vS1_MET.get(i);

        if (_bIsCovR == true) { // CovR
            lamda = std::sqrt(lamda);
        }

        mRank_S1(i, i) = lamda;
    }

    itpp::cmat mRank_V1 = mV1_MET.get_cols(0, iRank - 1);

    _mL1_MET = mRank_S1 * mRank_V1.H();
//    if(Parameters::Instance().MIMO_CTRL.IEnable_FDD ==1 ){
//        //直接利用反馈的CQI中的特征值进行计算
////
////        for (int i = 0; i < iRank; i++) {
////            double lamda = vS1_MET.get(i);
////
////            if (_bIsCovR == true) { // CovR
////                lamda = std::sqrt(lamda);
////            }
////
////            mRank_S1(i, i) = lamda;
////        }
////
////        itpp::cmat mRank_V1 = mV1_MET.get_cols(0, iRank - 1);
////
////        _mL1_MET = mRank_S1 * mRank_V1.H();
//
//    }
//    else{
//        //wxd:获取特征值及其对应的特征向量
//        itpp::cmat mU1_MET, mV1_MET;
//        itpp::vec vS1_MET;
//        itpp::svd(mH1_MET, mU1_MET, vS1_MET, mV1_MET);
//
//        itpp::mat mRank_S1 = zeros(iRank, iRank);
//        for (int i = 0; i < iRank; i++) {
//            double lamda = vS1_MET.get(i);
//
//            if (_bIsCovR == true) { // CovR
//                lamda = std::sqrt(lamda);
//            }
//
//            mRank_S1(i, i) = lamda;
//        }
//
//        itpp::cmat mRank_V1 = mV1_MET.get_cols(0, iRank - 1);
//
//        _mL1_MET = mRank_S1 * mRank_V1.H();
//    }

}
void CalcL_for_MET_RankA(
        itpp::cmat& mH1_MET, int iRank, itpp::cmat& _mL1_MET, bool _bIsCovR) {

    // rank start from 1

    itpp::cmat mU1_MET, mV1_MET;
    itpp::vec vS1_MET;
    itpp::svd(mH1_MET, mU1_MET, vS1_MET, mV1_MET);

    itpp::mat mRank_S1 = zeros(iRank, iRank);
    for (int i = 0; i < iRank; i++) {
        double lamda = vS1_MET.get(i);

        if (_bIsCovR) { // CovR
            lamda = std::sqrt(lamda);
        }

        mRank_S1(i, i) = lamda;
    }

    itpp::cmat mRank_V1 = mV1_MET.get_cols(0, iRank - 1);

    _mL1_MET = mRank_S1 * mRank_V1.H();
}

void test_4for_MET_RankA() {
    int mR = 2;
    int nT = 8;
    int M = 8;
    int maxRank = 0;
    bool _bIsCovR;

    vector<cmat> vHs(M, itpp::zeros_c(mR, nT));
    vector<cmat> vRs(M, itpp::zeros_c(nT, nT));

    vector<cmat> _vCodeWord(M, itpp::zeros_c(nT, maxRank));
    vector<int> _vRank(M, 0);

    for (int i = 0; i < M; i++) {
        vHs[i] = itpp::randn_c(mR, nT);
        vRs[i] = vHs[i].H() * vHs[i];

        _vRank[i] = maxRank;
    }

    _bIsCovR = true;
    MET_RankA(vRs, _vCodeWord, _vRank, _bIsCovR);

    _bIsCovR = false;
    MET_RankA(vHs, _vCodeWord, _vRank, _bIsCovR);


}

void test_4CalcL_for_MET_RankA() {
    int mR = 2;
    int nT = 8;

    itpp::cmat mH1_MET = itpp::randn_c(mR, nT);
    int iRank = 0;
    itpp::cmat _mL1_MET;

    bool _bIsCovR = false;
    CalcL_for_MET_RankA(
            mH1_MET, iRank, _mL1_MET, _bIsCovR);

    _bIsCovR = true;
    mH1_MET = mH1_MET.H() * mH1_MET;
    CalcL_for_MET_RankA(
            mH1_MET, iRank, _mL1_MET, _bIsCovR);
}

/*
void test_(){
    
    MET_RankA(
        const vector<cmat>& _vH, 
        vector<cmat>& _vCodeWord, 
        vector<int>& _vRank, bool _bIsCovR) ;
}
 */



//
//cmat ProductmPPortleftsub() {
//    int iHorizontalAntNum = cm::P::s().MacroTX.IHTotalAntNum;
//    int iElevationAntNum = (cm::P::s().MacroTX.ITotalAntNum) / (cm::P::s().MacroTX.IHTotalAntNum);
//    cmat mPPortleft;
//    if (Parameters::Instance().MIMO_CTRL.I2Dor3DMIMO == 0) {
//        if (Parameters::Instance().MIMO_CTRL.SubarrORFullconnec == 0) {
//            switch (Parameters::Instance().MIMO_CTRL.I2DMatrixmode) {
//                case 0://64*8
//                {
//                    mPPortleft = itpp::ones_c(cm::P::s().MacroTX.ITotalAntNum, cm::P::s().MacroTX.IHTotalAntNum); //对角阵叠加
//                    cmat unitmatrix = itpp::eye_c(cm::P::s().MacroTX.IHTotalAntNum); //单位矩阵
//                    for (int m = 0; m < iElevationAntNum; ++m) {
//                        cmat mPPortleftsub = sqrt(1.0 / iElevationAntNum) * exp(-1 * cm::M_J * 2 * M_PI * m * cm::P::s().MacroTX.DVAntSpace * cos(cm::DEG2RAD(Parameters::Instance().Macro.DElectricalTiltDeg) + M_PI / 2)) * unitmatrix;
//                        mPPortleft.set_submatrix(m * cm::P::s().MacroTX.IHTotalAntNum, (m + 1) * cm::P::s().MacroTX.IHTotalAntNum - 1, 0, cm::P::s().MacroTX.IHTotalAntNum - 1, mPPortleftsub);
//                    }
//                }
//                    break;
//
//                case 1://64*16
//                {
//                    mPPortleft = itpp::ones_c(64, 16);
//                    cmat mPPortleftsub = itpp::ones_c(32, 8);
//                    cmat unitmatrix = itpp::eye_c(8); //单位矩阵
//                    cmat zeromatrix = itpp::zeros_c(32, 8);
//                    for (int m = 0; m < 4; ++m) {
//                        cmat mPPortleftsub_one = sqrt(1.0 / 4) * exp(-1 * cm::M_J * 2 * M_PI * m * cm::P::s().MacroTX.DVAntSpace * cos(cm::DEG2RAD(Parameters::Instance().Macro.DElectricalTiltDeg) + M_PI / 2)) * unitmatrix;
//                        mPPortleftsub.set_submatrix(m * 8, (m + 1) * 8 - 1, 0, 7, mPPortleftsub_one);
//                    }
//                    mPPortleft.set_submatrix(0, 31, 0, 7, mPPortleftsub);
//                    mPPortleft.set_submatrix(0, 31, 8, 15, zeromatrix);
//                    mPPortleft.set_submatrix(32, 63, 0, 7, zeromatrix);
//                    mPPortleft.set_submatrix(32, 63, 8, 15, mPPortleftsub);
//                }
//                    break;
//
//                case 2://64*32
//                {
//                    mPPortleft = itpp::zeros_c(64, 32);
//                    cmat mPPortleftsub = itpp::ones_c(16, 8);
//                    cmat unitmatrix = itpp::eye_c(8); //单位矩阵
//                    for (int m = 0; m < 2; ++m) {
//                        cmat mPPortleftsub_one = sqrt(1.0 / 2) * exp(-1 * cm::M_J * 2 * M_PI * m * cm::P::s().MacroTX.DVAntSpace * cos(cm::DEG2RAD(Parameters::Instance().Macro.DElectricalTiltDeg) + M_PI / 2)) * unitmatrix;
//                        mPPortleftsub.set_submatrix(m * 8, (m + 1) * 8 - 1, 0, 7, mPPortleftsub_one);
//                    }
//                    mPPortleft.set_submatrix(0, 15, 0, 7, mPPortleftsub);
//                    mPPortleft.set_submatrix(16, 31, 8, 15, mPPortleftsub);
//                    mPPortleft.set_submatrix(32, 47, 16, 23, mPPortleftsub);
//                    mPPortleft.set_submatrix(48, 63, 24, 31, mPPortleftsub);
//                }
//                    break;
//                case 3://128*16
//                {
//                    mPPortleft = itpp::ones_c(128, 16); //对角阵叠加
//                    cmat unitmatrix = itpp::eye_c(16); //单位矩阵
//                    for (int m = 0; m < 8; ++m) {
//                        cmat mPPortleftsub = sqrt(1.0 / 8) * exp(-1 * cm::M_J * 2 * M_PI * m * cm::P::s().MacroTX.DVAntSpace * cos(cm::DEG2RAD(Parameters::Instance().Macro.DElectricalTiltDeg) + M_PI / 2)) * unitmatrix;
//                        mPPortleft.set_submatrix(m * 16, (m + 1) * 16 - 1, 0, 15, mPPortleftsub);
//                    }
//                }
//                    break;
//                case 4://128*32
//                {
//                    mPPortleft = itpp::ones_c(128, 32);
//                    cmat mPPortleftsub = itpp::ones_c(64, 16);
//                    cmat unitmatrix = itpp::eye_c(16); //单位矩阵
//                    cmat zeromatrix = itpp::zeros_c(64, 16);
//                    for (int m = 0; m < 4; ++m) {
//                        cmat mPPortleftsub_one = sqrt(1.0 / 4) * exp(-1 * cm::M_J * 2 * M_PI * m * cm::P::s().MacroTX.DVAntSpace * cos(cm::DEG2RAD(Parameters::Instance().Macro.DElectricalTiltDeg) + M_PI / 2)) * unitmatrix;
//                        mPPortleftsub.set_submatrix(m * 16, (m + 1) * 16 - 1, 0, 15, mPPortleftsub_one);
//                    }
//                    mPPortleft.set_submatrix(0, 63, 0, 15, mPPortleftsub);
//                    mPPortleft.set_submatrix(0, 63, 16, 31, zeromatrix);
//                    mPPortleft.set_submatrix(64, 127, 0, 15, zeromatrix);
//                    mPPortleft.set_submatrix(64, 127, 16, 31, mPPortleftsub);
//                }
//                    break;
//
//                case 5://128*64
//                {
//                    mPPortleft = itpp::zeros_c(128, 64);
//                    cmat mPPortleftsub = itpp::ones_c(32, 16);
//                    cmat unitmatrix = itpp::eye_c(16); //单位矩阵
//                    cmat zeromatrix = itpp::zeros_c(32, 16);
//                    for (int m = 0; m < 2; ++m) {
//                        cmat mPPortleftsub_one = sqrt(1.0 / 2) * exp(-1 * cm::M_J * 2 * M_PI * m * cm::P::s().MacroTX.DVAntSpace * cos(cm::DEG2RAD(Parameters::Instance().Macro.DElectricalTiltDeg) + M_PI / 2)) * unitmatrix;
//                        mPPortleftsub.set_submatrix(m * 16, (m + 1) * 16 - 1, 0, 15, mPPortleftsub_one);
//                    }
//                    mPPortleft.set_submatrix(0, 31, 0, 15, mPPortleftsub);
//                    mPPortleft.set_submatrix(32, 63, 16, 31, mPPortleftsub);
//                    mPPortleft.set_submatrix(64, 95, 32, 47, mPPortleftsub);
//                    mPPortleft.set_submatrix(96, 127, 48, 63, mPPortleftsub);
//                }
//                    break;
//                case 6://96*24
//                {
//                    mPPortleft = itpp::zeros_c(96, 24);
//                    cmat mPPortleftsub = itpp::ones_c(48, 12);
//                    cmat unitmatrix = itpp::eye_c(12); //单位矩阵
//                    for (int m = 0; m < 4; ++m) {
//                        cmat mPPortleftsub_one = sqrt(1.0 / 4) * exp(-1 * cm::M_J * 2 * M_PI * m * cm::P::s().MacroTX.DVAntSpace * cos(cm::DEG2RAD(Parameters::Instance().Macro.DElectricalTiltDeg) + M_PI / 2)) * unitmatrix;
//                        mPPortleftsub.set_submatrix(m * 12, (m + 1) * 12 - 1, 0, 11, mPPortleftsub_one);
//                    }
//                    mPPortleft.set_submatrix(0, 47, 0, 11, mPPortleftsub);
//                    mPPortleft.set_submatrix(48, 95, 12, 23, mPPortleftsub);
//                }
//                    break;
//                case 7://96*32
//                {
//                    mPPortleft = itpp::zeros_c(96, 32);
//                    cmat mPPortleftsub = itpp::ones_c(24, 8);
//                    cmat unitmatrix = itpp::eye_c(8); //单位矩阵
//                    for (int m = 0; m < 3; ++m) {
//                        cmat mPPortleftsub_one = sqrt(1.0 / 3) * exp(-1 * cm::M_J * 2 * M_PI * m * cm::P::s().MacroTX.DVAntSpace * cos(cm::DEG2RAD(Parameters::Instance().Macro.DElectricalTiltDeg) + M_PI / 2)) * unitmatrix;
//                        mPPortleftsub.set_submatrix(m * 8, (m + 1) * 8 - 1, 0, 7, mPPortleftsub_one);
//                    }
//                    mPPortleft.set_submatrix(0, 23, 0, 7, mPPortleftsub);
//                    mPPortleft.set_submatrix(24, 47, 8, 15, mPPortleftsub);
//                    mPPortleft.set_submatrix(48, 71, 16, 23, mPPortleftsub);
//                    mPPortleft.set_submatrix(72, 95, 24, 31, mPPortleftsub);
//                }
//                    break;
//                case 8://192*64
//                {
//                    mPPortleft = itpp::zeros_c(192, 64);
//                    cmat mPPortleftsub = itpp::ones_c(48, 16);
//                    cmat unitmatrix = itpp::eye_c(16); //单位矩阵
//                    for (int m = 0; m < 3; ++m) {
//                        cmat mPPortleftsub_one = sqrt(1.0 / 3) * exp(-1 * cm::M_J * 2 * M_PI * m * cm::P::s().MacroTX.DVAntSpace * cos(cm::DEG2RAD(Parameters::Instance().Macro.DElectricalTiltDeg) + M_PI / 2)) * unitmatrix;
//                        mPPortleftsub.set_submatrix(m * 16, (m + 1) * 16 - 1, 0, 15, mPPortleftsub_one);
//                    }
//                    mPPortleft.set_submatrix(0, 47, 0, 15, mPPortleftsub);
//                    mPPortleft.set_submatrix(48, 95, 16, 31, mPPortleftsub);
//                    mPPortleft.set_submatrix(96, 143, 32, 47, mPPortleftsub);
//                    mPPortleft.set_submatrix(144, 191, 48, 63, mPPortleftsub);
//                }
//                    break;
//                case 9://16*2 for mMTC-ConfigA
//                {
//                    mPPortleft = itpp::zeros_c(16, 2);
//                    //cmat mPPortleftsub = itpp::ones_c(48, 16);
//                    cmat unitmatrix = itpp::eye_c(2); //单位矩阵
////                    for (int m = 0; m < 3; ++m) {
////                        cmat mPPortleftsub_one = sqrt(1.0 / 3) * exp(-1 * cm::M_J * 2 * M_PI * m * cm::P::s().MacroTX.DVAntSpace * cos(cm::DEG2RAD(Parameters::Instance().Macro.DElectricalTiltDeg) + M_PI / 2)) * unitmatrix;
////                        mPPortleft.set_submatrix(m * 2, (m + 1) * 2 - 1, 0, 1, mPPortleftsub_one);
////                    }
//                    for (int m = 0; m < 8; ++m) {
//                        cmat mPPortleftsub_one = sqrt(1.0 / 8) * exp(-1 * cm::M_J * 2 * M_PI * m * cm::P::s().MacroTX.DVAntSpace * cos(cm::DEG2RAD(Parameters::Instance().Macro.DElectricalTiltDeg) + M_PI / 2)) * unitmatrix;
//                        mPPortleft.set_submatrix(m * 2, (m + 1) * 2 - 1, 0, 1, mPPortleftsub_one);
//                    }
//                    
//                    
//                }
//                    break;
//                default:
//                    assert(false);
//                    break;
//            }
//        } else if (Parameters::Instance().MIMO_CTRL.SubarrORFullconnec == 1) {
//            switch (Parameters::Instance().MIMO_CTRL.I2DMatrixmode) {
//                case 0:
//                {
//                    mPPortleft = itpp::ones_c(cm::P::s().MacroTX.ITotalAntNum, cm::P::s().MacroTX.IHTotalAntNum); //对角阵叠加
//                    cmat unitmatrix = itpp::eye_c(cm::P::s().MacroTX.IHTotalAntNum); //单位矩阵
//                    for (int m = 0; m < iElevationAntNum; ++m) {
//                        cmat mPPortleftsub = sqrt(1.0 / iElevationAntNum) * exp(-1 * cm::M_J * 2 * M_PI * m * cm::P::s().MacroTX.DVAntSpace * cos(cm::DEG2RAD(Parameters::Instance().Macro.DElectricalTiltDeg) + M_PI / 2)) * unitmatrix;
//                        mPPortleft.set_submatrix(m * cm::P::s().MacroTX.IHTotalAntNum, (m + 1) * cm::P::s().MacroTX.IHTotalAntNum - 1, 0, cm::P::s().MacroTX.IHTotalAntNum - 1, mPPortleftsub);
//                    }
//                }
//                    break;
//
//                case 1:
//                {
//                    mPPortleft = itpp::ones_c(64, 16);
//                    int K = 8;
//                    vector<double> vDownDeg;
//                    for (int i = 0; i < 2; i++) {
//                        vDownDeg.push_back(-10 + i * 20);
//                    }
//                    cmat mPPortleftsub = itpp::ones_c(64, 8);
//                    cmat unitmatrix = itpp::eye_c(8); //单位矩阵
//                    cmat zeromatrix = itpp::zeros_c(32, 8);
//                    for (int i = 0; i < static_cast<int> (vDownDeg.size()); i++) {
//                        for (int m = 0; m < K; ++m) {
//                            cmat mPPortleftsub_one = sqrt(1.0 / K) * exp(-1 * cm::M_J * 2 * M_PI * m * cm::P::s().MacroTX.DVAntSpace * cos(cm::DEG2RAD(vDownDeg[i]) + M_PI / 2)) * unitmatrix;
//                            mPPortleftsub.set_submatrix(m * 8, (m + 1) * 8 - 1, 0, 7, mPPortleftsub_one);
//                        }
//                        mPPortleft.set_submatrix(0, 63, i * 8, (i + 1) * 8 - 1, mPPortleftsub);
//                    }
//                }
//                    break;
//
//                case 2:
//                {
//                    mPPortleft = itpp::zeros_c(64, 32);
//                    int K = 8; //每次一列8个天线映射
//                    vector<double> vDownDeg;
//                    for (int i = 0; i < 4; i++) {
//                        vDownDeg.push_back(-10 + i * 10);
//                    }
//                    cmat mPPortleftsub = itpp::ones_c(64, 8);
//                    cmat unitmatrix = itpp::eye_c(8); //单位矩阵
//                    for (int i = 0; i < static_cast<int> (vDownDeg.size()); i++) {
//                        for (int m = 0; m < K; ++m) {
//                            cmat mPPortleftsub_one = sqrt(1.0 / K) * exp(-1 * cm::M_J * 2 * M_PI * m * cm::P::s().MacroTX.DVAntSpace * cos(cm::DEG2RAD(vDownDeg[i]) + M_PI / 2)) * unitmatrix;
//                            mPPortleftsub.set_submatrix(m * 8, (m + 1) * 8 - 1, 0, 7, mPPortleftsub_one);
//                        }
//                        mPPortleft.set_submatrix(0, 63, i * 8, (i + 1) * 8 - 1, mPPortleftsub);
//                    }
//                }
//                    break;
//            }
//        }
//    } else if (Parameters::Instance().MIMO_CTRL.I2Dor3DMIMO == 1) {
//        if (Parameters::Instance().MIMO_CTRL.SubarrORFullconnec == 0) {
//            mPPortleft = itpp::eye_c(cm::P::s().MacroTX.ITotalAntNum); //单位矩阵
//        } else if (Parameters::Instance().MIMO_CTRL.SubarrORFullconnec == 1) {
//            cmat unitmatrix = itpp::eye_c(8); //单位矩阵
//            mPPortleft = itpp::zeros_c(64, 64);
//            vector<double> vDownDeg;
//            for (int i = 0; i < 8; i++) {
//                vDownDeg.push_back(-15 + i * 5);
//            }
//            cmat mPPortleftsub = itpp::ones_c(64, 8); //对角阵叠加,64x8
//            int K = 8; //每次都是8个映射
//            for (int i = 0; i < static_cast<int> (vDownDeg.size()); i++) {
//                for (int m = 0; m < K; ++m) {
//                    cmat mPPortleftsub_one = sqrt(1.0 / K) * exp(-1 * cm::M_J * 2 * M_PI * m * cm::P::s().MacroTX.DVAntSpace * cos(cm::DEG2RAD(vDownDeg[i]) + M_PI / 2)) * unitmatrix;
//                    mPPortleftsub.set_submatrix(m * 8, (m + 1) * 8 - 1, 0, 7, mPPortleftsub_one);
//                }
//                mPPortleft.set_submatrix(0, 63, 8 * i, (i + 1)*8 - 1, mPPortleftsub);
//            }
//        }
//    }
//
//    return mPPortleft;
//}

cmat ProductPrecodeMat(const cmat& mTemp) {
    //    itpp::cmat mPPortleft = ProductmPPortleftsub();
    //20171206
    double detiltRAD = cm::DEG2RAD(Parameters::Instance().Macro.DElectricalTiltDeg);
    double descanRAD = 0;
    cmat mPPortleft = ProductmPPortleftsub_for_TXRU(
            detiltRAD, descanRAD,
            Parameters::Instance().Macro.IHAntNumPerPanel,
            Parameters::Instance().Macro.IVAntNumPerPanel,
            Parameters::Instance().Macro.IHPanelNum,
            Parameters::Instance().Macro.IVPanelNum,
            2);

    int row = mPPortleft.cols();
    itpp::cmat mV = mTemp.get_cols(0, 0);
    itpp::cmat PrecodeMat = itpp::zeros_c(row, Parameters::Instance().MIMO_CTRL.BF_CSIRS_PortNum);
    if (Parameters::Instance().MIMO_CTRL.BF_CSIRS_PortNum == 2) {
        itpp::cmat mV1 = itpp::zeros_c(row, 1);
        itpp::cmat mV2 = itpp::zeros_c(row, 1);
        for (int i = 0; i < (row / 2); i++) {
            mV1(2 * i, 0) = mV(i, 0);
            mV2((2 * i + 1), 0) = mV(i, 0);
        }
        PrecodeMat.set_submatrix(0, row - 1, 0, 0, mV1);
        PrecodeMat.set_submatrix(0, row - 1, 1, 1, mV2);
    } else if (Parameters::Instance().MIMO_CTRL.BF_CSIRS_PortNum == 4) {
        itpp::cmat mV1 = itpp::zeros_c(row, 1);
        itpp::cmat mV2 = itpp::zeros_c(row, 1);
        itpp::cmat mV3 = itpp::zeros_c(row, 1);
        itpp::cmat mV4 = itpp::zeros_c(row, 1);
        ///此映射只适合64port，4Rx接收
        for (int i = 0; i < 8; i++) {
            mV1(8 * i, 0) = mV(8 * i, 0);
            mV1(8 * i + 2, 0) = mV(8 * i + 2, 0);

            mV2(8 * i + 1, 0) = mV(8 * i + 1, 0);
            mV2(8 * i + 3, 0) = mV(8 * i + 3, 0);

            mV3(8 * i + 4, 0) = mV(8 * i + 4, 0);
            mV3(8 * i + 6, 0) = mV(8 * i + 6, 0);

            mV4(8 * i + 5, 0) = mV(8 * i + 5, 0);
            mV4(8 * i + 7, 0) = mV(8 * i + 7, 0);
        }
        PrecodeMat.set_submatrix(0, row - 1, 0, 0, mV1);
        PrecodeMat.set_submatrix(0, row - 1, 1, 1, mV2);
        PrecodeMat.set_submatrix(0, row - 1, 2, 2, mV3);
        PrecodeMat.set_submatrix(0, row - 1, 3, 3, mV4);
    } else {
        cout << "Wrong MSS Port!" << endl;
        abort();
    }
    return PrecodeMat;
}

cmat ChangeMatFull2Half(const cmat& InMatrix) {
    int col_nums = InMatrix.cols();
    int row_nums = InMatrix.rows();
    itpp::cmat mMatrix = itpp::zeros_c(row_nums, ceil(col_nums / 2.0));
    for (int i = 0; i < col_nums; i = i + 2) {
        mMatrix.set_submatrix(0, row_nums - 1, i / 2, i / 2, InMatrix.get_cols(i, i));
    }
    itpp::cmat OutMatrix = itpp::zeros_c(ceil(row_nums / 2.0), ceil(col_nums / 2.0));
    for (int i = 0; i < row_nums; i = i + 2) {
        OutMatrix.set_submatrix(i / 2, i / 2, 0, ceil(col_nums / 2.0) - 1, mMatrix.get_rows(i, i));
    }
    return OutMatrix;
}
//
//cmat ProductmPPortleftsub(const double _detiltRAD, const double _descanRAD, const int _iN_H_perPanel, const int _iN_V_perPanel, const int _iN_H_Total, const int _iN_V_Total, int PolarNum) {
//    ///注意：该函数中_detiltRAD=0代表垂直于Z轴方向；_descanRAD代表和水平维主瓣方向的角度差值
//    if (Parameters::Instance().Macro.DL.bEnable_BSAnalogBF) {
//        // mWeightPerPanel = [h0v0, h0v1, h0v2, ..., h1v0, h1v1, h1v2, ...], 标准
//        cmat mWeightPerPanel = cm::CalPanelWeight(_detiltRAD, _descanRAD, _iN_H_perPanel, _iN_V_perPanel); //矩阵天线加权可以取的值，验证时可以注释掉
//        
//        int iTotalAntNum = _iN_H_Total * _iN_V_Total * PolarNum;
//        
//        int iAntNumPerPanel = _iN_H_perPanel * _iN_V_perPanel;
//        assert(iAntNumPerPanel == mWeightPerPanel.cols());
//        
//        int iPortNum = iTotalAntNum / iAntNumPerPanel; //天线映射的通道数
//        
//        cmat matrix = itpp::zeros_c(iTotalAntNum, iPortNum); //最终返回的矩阵
//        //171129
//        
//        int H_Panel_Num = _iN_H_Total / _iN_H_perPanel;
//        int V_Panel_Num = _iN_V_Total / _iN_V_perPanel;
//
//        for (int V_Panel_index = 0; V_Panel_index < V_Panel_Num; V_Panel_index++) {
//            int PortNum_of_one_layer_H_panel_with_Polar = H_Panel_Num * PolarNum;
//            int AntNum_of_one_layer_H_panel_with_Polar = iAntNumPerPanel * PortNum_of_one_layer_H_panel_with_Polar;
//
//            int iStartAntIndex_V = V_Panel_index * AntNum_of_one_layer_H_panel_with_Polar;  
//                
//            for (int H_Panel_index = 0; H_Panel_index < H_Panel_Num; H_Panel_index++) {
//                int AntNum_of_one_panel_with_Polar = _iN_H_perPanel * PolarNum;
//                
//                int iStartAntIndex_V_2 = iStartAntIndex_V
//                    + H_Panel_index * AntNum_of_one_panel_with_Polar;
//                
//                for (int Polar_index = 0; Polar_index < PolarNum; Polar_index++) {
//                    int iStartAntIndex_V_3 = iStartAntIndex_V_2 + Polar_index;
//                    
//                    int PortIndex = Polar_index + H_Panel_index * PolarNum 
//                        + V_Panel_index * H_Panel_Num * PolarNum;
//
//                    for (int iVIndex = 0; iVIndex < _iN_V_perPanel; ++iVIndex) {
//                        for (int iHIndex = 0; iHIndex < _iN_H_perPanel; ++iHIndex) {
//                            int iAntIndex = iStartAntIndex_V_3 
//                                    + PolarNum * iHIndex
//                                    + _iN_H_Total * PolarNum * iVIndex;
//                            
//                            // mWeightPerPanel = [h0v0, h0v1, h0v2, ..., h1v0, h1v1, h1v2, ...], 标准
////                            matrix(iAntIndex, PortIndex) 
////                                    = mWeightPerPanel(0, iVIndex + iHIndex * _iN_V_perPanel);                       
//                       
//                            matrix(iAntIndex, PortIndex) 
//                                = mWeightPerPanel(0, 
//                                    cm::GetPanelWeightIndex(iHIndex, iVIndex, _iN_H_perPanel, _iN_V_perPanel));                       
//                     
//                        }
//                    }
//                }
//            }
//        }
//
//        
//        return matrix;
//    } else {
//        cmat matrix = ProductmPPortleftsub();
//        return matrix;
//    }
//}

cmat ProductmPPortleftsub(const pair<int, vector<int> >& _PanelAndvBeamIndex, const int _iN_H_perPanel, const int _iN_V_perPanel, const int _iN_H_Total, const int _iN_V_Total) {
    ///注意：该函数中_detiltRAD=0代表垂直于Z轴方向；_descanRAD代表和水平维主瓣方向的角度差值
    if (Parameters::Instance().Macro.DL.bEnable_BSAnalogBF) {
        cmat matrix = ProductmPPortleftsub(_PanelAndvBeamIndex.second, _iN_H_perPanel, _iN_V_perPanel, _iN_H_Total, _iN_V_Total);
        int iPanelIndicator = _PanelAndvBeamIndex.first;
        cmat result = itpp::zeros_c(matrix.rows(), matrix.cols());
        if (iPanelIndicator == 0) {//Panel 0&2
            result.set_submatrix(0, matrix.rows() / 2 - 1, 0, matrix.cols() / 4 - 1,
                    matrix.get(0, matrix.rows() / 2 - 1, 0, matrix.cols() / 4 - 1));
            result.set_submatrix(matrix.rows() / 2, matrix.rows() - 1, matrix.cols() / 2, matrix.cols()*3 / 4 - 1,
                    matrix.get(matrix.rows() / 2, matrix.rows() - 1, matrix.cols() / 2, matrix.cols()*3 / 4 - 1));
        } else if (iPanelIndicator == 1) {//Panel 1&3
            result.set_submatrix(0, matrix.rows() / 2 - 1, matrix.cols() / 4, matrix.cols() / 2 - 1,
                    matrix.get(0, matrix.rows() / 2 - 1, matrix.cols() / 4, matrix.cols() / 2 - 1));
            result.set_submatrix(matrix.rows() / 2, matrix.rows() - 1, matrix.cols()*3 / 4, matrix.cols() - 1,
                    matrix.get(matrix.rows() / 2, matrix.rows() - 1, matrix.cols()*3 / 4, matrix.cols() - 1));
        } else if (iPanelIndicator == 2) {//Panel 0&1&2&3
            result = matrix;
        } else {
            cout << "iPanelIndicator error!" << endl;
            assert(false);
        }
        return result;
    } else {
        //        cmat result = ProductmPPortleftsub();
        //20171206
        double detiltRAD = cm::DEG2RAD(Parameters::Instance().Macro.DElectricalTiltDeg);
        double descanRAD = 0;
        cmat result = ProductmPPortleftsub_for_TXRU(
                detiltRAD, descanRAD,
                Parameters::Instance().Macro.IHAntNumPerPanel,
                Parameters::Instance().Macro.IVAntNumPerPanel,
                Parameters::Instance().Macro.IHPanelNum,
                Parameters::Instance().Macro.IVPanelNum,
                2);


        return result;
    }
}

cmat ProductmPPortleftsub(const vector<int>& vSelectedBeamIndex, const int _iN_H_perPanel, const int _iN_V_perPanel, const int _iN_H_Total, const int _iN_V_Total) {
    ///注意：该函数中_detiltRAD=0代表垂直于Z轴方向；_descanRAD代表和水平维主瓣方向的角度差值
    int iMUNum = vSelectedBeamIndex.size();
    int iAntNumPerPanel = _iN_H_perPanel*_iN_V_perPanel;
    int iPortNum = _iN_H_Total * _iN_V_Total * 2 / iAntNumPerPanel; //天线映射的通道数
    cmat result = itpp::zeros_c(_iN_H_Total * _iN_V_Total * 2, iPortNum);
    for (int i = 0; i != iMUNum; i++) {
        int iBSBeamIndex = vSelectedBeamIndex[i];
        double detiltRAD = BSBeamIndex2EtiltRAD(iBSBeamIndex);
        double descanRAD = BSBeamIndex2EscanRAD(iBSBeamIndex);
        //        cmat matrix = ProductmPPortleftsub(detiltRAD, descanRAD, _iN_H_perPanel, _iN_V_perPanel, _iN_H_Total, _iN_V_Total);
        //20171206
        cmat matrix = ProductmPPortleftsub_for_TXRU(
                detiltRAD, descanRAD,
                _iN_H_perPanel, _iN_V_perPanel,
                _iN_H_Total / _iN_H_perPanel, _iN_V_Total / _iN_V_perPanel);



        result += matrix;
    }
    result = result / sqrt(iMUNum);
    return result;
}

double BSBeamIndex2EtiltRAD(int _iBSBeamIndex) {
    int iHBSBeamNum = Parameters::Instance().Macro.ANALOGBEAM_CONFIG.iHBSBeamNum;
    vector<double> vBSetiltRAD = Parameters::Instance().Macro.ANALOGBEAM_CONFIG.vBSetiltRAD;
    int iVBSBeamIndex = _iBSBeamIndex / iHBSBeamNum;
    double detiltRAD = vBSetiltRAD[iVBSBeamIndex];
    return detiltRAD;
}

double BSBeamIndex2EscanRAD(int _iBSBeamIndex) {
    int iVBSBeamNum = Parameters::Instance().Macro.ANALOGBEAM_CONFIG.iVBSBeamNum;
    int iHBSBeamNum = Parameters::Instance().Macro.ANALOGBEAM_CONFIG.iHBSBeamNum;
    vector<double> vBSescanRAD = Parameters::Instance().Macro.ANALOGBEAM_CONFIG.vBSescanRAD;
    int iHBSBeamIndex = _iBSBeamIndex % iHBSBeamNum;
    double descanRAD = vBSescanRAD[iHBSBeamIndex];
    return descanRAD;
}

double UEBeamIndex2EtiltRAD(int _iUEBeamIndex) {
    int iVUEBeamNum = Parameters::Instance().Macro.ANALOGBEAM_CONFIG.iVUEBeamNum;
    int iHUEBeamNum = Parameters::Instance().Macro.ANALOGBEAM_CONFIG.iHUEBeamNum;
    vector<double> vUEetiltRAD = Parameters::Instance().Macro.ANALOGBEAM_CONFIG.vUEetiltRAD;
    int iVUEBeamIndex = _iUEBeamIndex / iHUEBeamNum;
    double detiltRAD = vUEetiltRAD[iVUEBeamIndex];
    return detiltRAD;
}

double UEBeamIndex2EscanRAD(int _iUEBeamIndex) {
    int iVUEBeamNum = Parameters::Instance().Macro.ANALOGBEAM_CONFIG.iVUEBeamNum;
    int iHUEBeamNum = Parameters::Instance().Macro.ANALOGBEAM_CONFIG.iHUEBeamNum;
    vector<double> vUEescanRAD = Parameters::Instance().Macro.ANALOGBEAM_CONFIG.vUEescanRAD;
    int iHUEBeamIndex = _iUEBeamIndex % iHUEBeamNum;
    double descanRAD = vUEescanRAD[iHUEBeamIndex];
    return descanRAD;
}

cmat ProductmPPortleft_UE(double dUEetiltRAD, double dUEescanRAD) {
    cmat mPPortleft_UE;

    int IHAntNumPerPanel = Parameters::Instance().MSS.FirstBand.IHAntNumPerPanel;
    int IVAntNumPerPanel = Parameters::Instance().MSS.FirstBand.IVAntNumPerPanel;

    int IHPanelNum = Parameters::Instance().MSS.FirstBand.IHPanelNum; // 水平Panel数为2时，视为正反面板;为1时,只有一个面板
    int IVPanelNum = Parameters::Instance().MSS.FirstBand.IVPanelNum;

    int PolarNum = Parameters::Instance().MSS.FirstBand.Polarize_Num;

    //    if (Parameters::Instance().MSS.FirstBand.bEnable_UEAnalogBF){
    //        mPPortleft_UE = (ProductmPPortleftsub(dUEetiltRAD, dUEescanRAD, 4, 2, 4, 2)).T();        

    // 水平Panel数为2时，视为正反面板，构造Left矩阵时，只考虑1个面板        
    if (IHPanelNum == 2)
        IHPanelNum = 1;

    mPPortleft_UE = (
            ProductmPPortleftsub_for_TXRU(
            dUEetiltRAD, dUEescanRAD,
            IHAntNumPerPanel,
            IVAntNumPerPanel,
            IHPanelNum,
            IVPanelNum,
            PolarNum,
            Parameters::Instance().MSS.FirstBand.DHAntSpace,
            Parameters::Instance().MSS.FirstBand.DVAntSpace)).T();

    return mPPortleft_UE;
}

//cmat operator*(const cmat &m1, const cmat &m2) {
//    assert( m1.cols() == m2.rows() );
//
//    cmat r(m1.rows(), m2.cols());
//
//    std::complex<double> tmp;
//
//    int i, j, k, r_pos = 0, pos = 0, m_pos = 0;
//
//    for (i = 0; i < r.cols(); i++) {
//        for (j = 0; j < r.rows(); j++) {
//            tmp = std::complex<double>(0.0);
//
//            for (k = 0; k < m1.cols(); k++) {
//                tmp += m1(j, k) * m2(k, i);
//            }
//            r(j, i) = tmp;
//        }
//    }
//
//    return r;
//}

//20171206

cmat ProductmPPortleftsub_for_TXRU(
        const double _detiltRAD, const double _descanRAD,
        const int _H_AntNumPerTXRU, const int _V_AntNumPerTXRU,
        const int _H_TXRU_Num, const int _V_TXRU_Num,
        int _PolarNum, double _DHAntSpace, double _DVAntSpace) {

    cmat mWeightPerPanel
            = cm::CalPanelWeight(
            _detiltRAD, _descanRAD, _H_AntNumPerTXRU, _V_AntNumPerTXRU,
            _DHAntSpace, _DVAntSpace); //矩阵天线加权可以取的值，验证时可以注释掉


    int iTotalAntNum
            = _H_AntNumPerTXRU * _H_TXRU_Num
            * _V_AntNumPerTXRU * _V_TXRU_Num * _PolarNum;

    int iAntNumPerTXRU = _H_AntNumPerTXRU * _V_AntNumPerTXRU;
    assert(iAntNumPerTXRU == mWeightPerPanel.cols());

    int iPortNum = _H_TXRU_Num * _V_TXRU_Num * _PolarNum; //天线映射的通道数

    cmat matrix = itpp::zeros_c(iTotalAntNum, iPortNum); //最终返回的矩阵

    int _iN_H_Total = _H_AntNumPerTXRU * _H_TXRU_Num;

    for (int V_Panel_index = 0; V_Panel_index < _V_TXRU_Num; V_Panel_index++) {
        int PortNum_of_one_layer_H_panel_with_Polar = _H_TXRU_Num * _PolarNum;
        int AntNum_of_one_layer_H_panel_with_Polar = iAntNumPerTXRU * PortNum_of_one_layer_H_panel_with_Polar;

        int iStartAntIndex_V = V_Panel_index * AntNum_of_one_layer_H_panel_with_Polar;

        for (int H_Panel_index = 0; H_Panel_index < _H_TXRU_Num; H_Panel_index++) {
            int AntNum_of_one_panel_with_Polar = _H_AntNumPerTXRU * _PolarNum;

            int iStartAntIndex_V_2 = iStartAntIndex_V
                    + H_Panel_index * AntNum_of_one_panel_with_Polar;

            for (int Polar_index = 0; Polar_index < _PolarNum; Polar_index++) {
                int iStartAntIndex_V_3 = iStartAntIndex_V_2 + Polar_index;

                int PortIndex = Polar_index + H_Panel_index * _PolarNum
                        + V_Panel_index * _H_TXRU_Num * _PolarNum;

                for (int iVIndex = 0; iVIndex < _V_AntNumPerTXRU; ++iVIndex) {
                    for (int iHIndex = 0; iHIndex < _H_AntNumPerTXRU; ++iHIndex) {
                        int iAntIndex = iStartAntIndex_V_3
                                + _PolarNum * iHIndex
                                + _iN_H_Total * _PolarNum * iVIndex;

                        // mWeightPerPanel = [h0v0, h0v1, h0v2, ..., h1v0, h1v1, h1v2, ...], 标准
                        //                            matrix(iAntIndex, PortIndex) 
                        //                                    = mWeightPerPanel(0, iVIndex + iHIndex * _iN_V_perPanel);                       

                        matrix(iAntIndex, PortIndex)
                                = mWeightPerPanel(0,
                                cm::GetPanelWeightIndex(iHIndex, iVIndex, _H_AntNumPerTXRU, _V_AntNumPerTXRU));

                    }
                }
            }
        }
    }


    return matrix;
}

