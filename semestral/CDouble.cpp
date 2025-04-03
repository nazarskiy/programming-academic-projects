#pragma once

#include "CDouble.h"

CDouble::CDouble ( double value ) : m_value ( value ) {}

CValue CDouble::evaluate ( const CSpreadsheet & spreadsheet, CValue lhs, CValue rhs ) const {
    return m_value;
}

bool CDouble::is_operator () const {
    return false;
}

void CDouble::mark ( std::ostream & os ) const {
    os << "double:";
}

void CDouble::print ( std::ostream & os ) const {
    os << m_value;
}

std::shared_ptr<CItem> CDouble::copy () const {
    return std::make_shared<CDouble> ( *this );
}