#pragma once

#include "CReference.h"

CReference::CReference ( const std::string & value ) : m_value ( value ) {}
// Method to evaluate the reference by fetching its value from the spreadsheet
CValue CReference::evaluate ( const CSpreadsheet & spreadsheet, CValue lhs, CValue rhs ) const {
    return spreadsheet.getValue ( m_value );
}

bool CReference::is_operator () const {
    return false;
}

const CPos & CReference::get_cpos () const {
    return m_value;
}

void CReference::move_row ( int offset ) {
    m_value.set_row ( m_value.get_row() - offset );
}

void CReference::move_column ( int offset ) {
    m_value.set_column ( m_value.get_column_numerical() - offset );
}

void CReference::mark ( std::ostream & os ) const {
    os << "reference:";
}

void CReference::print ( std::ostream & os) const {
    m_value.print ( os );
}

std::shared_ptr<CItem> CReference::copy () const {
    return std::make_shared<CReference> ( *this );
}