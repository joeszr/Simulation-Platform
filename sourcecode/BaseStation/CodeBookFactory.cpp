/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CodebookFactory.cpp
 * Author: zhenxiangsu
 * 
 * Created on 2020年1月6日, 下午3:35
 */

#include "CodeBookFactory.h"
#include "NR_CodeBook.h"

CodeBookFactory::CodeBookFactory()  {
	int _TxPortNum = 1;
	std::shared_ptr<CodeBook> _NR_CodeBook = std::make_shared<NR_CodeBook>(_TxPortNum);
	m_NR_CodeBook_map[_TxPortNum] = _NR_CodeBook;
	_TxPortNum = 2;
	_NR_CodeBook = std::make_shared<NR_CodeBook>(_TxPortNum);
	m_NR_CodeBook_map[_TxPortNum] = _NR_CodeBook;
	_TxPortNum = 4;
	_NR_CodeBook = std::make_shared<NR_CodeBook>(_TxPortNum);
	m_NR_CodeBook_map[_TxPortNum] = _NR_CodeBook;
	//1229
	_TxPortNum = 8;
	_NR_CodeBook = std::make_shared<NR_CodeBook>(_TxPortNum);
	m_NR_CodeBook_map[_TxPortNum] = _NR_CodeBook;

	_TxPortNum = 32;
	_NR_CodeBook = std::make_shared<NR_CodeBook>(_TxPortNum);
	m_NR_CodeBook_map[_TxPortNum] = _NR_CodeBook;
	/*
		m_NR_CodeBook_map.insert(pair<int, std::shared_ptr<CodeBook> >(2, _NR_CodeBook));
	 */
}

