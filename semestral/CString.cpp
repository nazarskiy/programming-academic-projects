#pragma once

#include "CString.h"

CString::CString ( std::string value ) : m_value ( std::move ( value ) ) {}

CValue CString::evaluate ( const CSpreadsheet & spreadsheet, CValue lhs, CValue rhs ) const {
    return m_value;
}

bool CString::is_operator () const {
    return false;
}

void CString::mark ( std::ostream & os ) const {
    os << "string:";
}

void CString::print ( std::ostream & os ) const {
    os << m_value;
}

std::shared_ptr<CItem> CString::copy () const {
    return std::make_shared<CString> ( *this );
}