#pragma once

#include "CMyBuilder.h"

const std::vector<std::shared_ptr<CItem>> & CMyBuilder::get_data () const {
    return m_expression;
}
// These methods add various types of operators and operands to the expression vector
void CMyBuilder::opAdd () {
    std::shared_ptr<CItem> new_item = std::make_shared<CAdd> ( 2 );
    m_expression.push_back ( new_item );
}

void CMyBuilder::opSub () {
    std::shared_ptr<CItem> new_item = std::make_shared<CSub> ( 2 );
    m_expression.push_back ( new_item );
}

void CMyBuilder::opMul ()  {
    std::shared_ptr<CItem> new_item = std::make_shared<CMul> ( 2 );
    m_expression.push_back ( new_item );
}

void CMyBuilder::opDiv ()  {
    std::shared_ptr<CItem> new_item = std::make_shared<CDiv> ( 2 );
    m_expression.push_back ( new_item );
}

void CMyBuilder::opPow ()  {
    std::shared_ptr<CItem> new_item = std::make_shared<CPow> ( 2 );
    m_expression.push_back ( new_item );
}

void CMyBuilder::opNeg ()  {
    std::shared_ptr<CItem> new_item = std::make_shared<CNeg> ( 1 );
    m_expression.push_back ( new_item );
}

void CMyBuilder::opEq () {
    std::shared_ptr<CItem> new_item = std::make_shared<CEq> ( 2 );
    m_expression.push_back ( new_item );
}

void CMyBuilder::opNe ()  {
    std::shared_ptr<CItem> new_item = std::make_shared<CNe> ( 2 );
    m_expression.push_back ( new_item );
}

void CMyBuilder::opLt()  {
    std::shared_ptr<CItem> new_item = std::make_shared<CLt> ( 2 );
    m_expression.push_back ( new_item );
}

void CMyBuilder::opLe ()  {
    std::shared_ptr<CItem> new_item = std::make_shared<CLe> ( 2 );
    m_expression.push_back ( new_item );
}

void CMyBuilder::opGt ()  {
    std::shared_ptr<CItem> new_item = std::make_shared<CGt> ( 2 );
    m_expression.push_back ( new_item );
}

void CMyBuilder::opGe ()  {
    std::shared_ptr<CItem> new_item = std::make_shared<CGe> ( 2 );
    m_expression.push_back ( new_item );
}

void CMyBuilder::valNumber ( double val )  {
    std::shared_ptr<CItem> new_item = std::make_shared<CDouble> ( val );
    m_expression.push_back ( new_item );
}

void CMyBuilder::valString ( std::string val )  {
    std::shared_ptr<CItem> new_item = std::make_shared<CString> ( val );
    m_expression.push_back ( new_item );
}

void CMyBuilder::valReference ( std::string val )  {
    std::shared_ptr<CItem> new_item = std::make_shared<CReference> ( val );
    m_expression.push_back ( new_item );
}

void CMyBuilder::valRange ( std::string val ) {}

void CMyBuilder::funcCall ( std::string fnName, int paramCount ) {}