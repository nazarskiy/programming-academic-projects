#pragma once

#include "CPow.h"

CPow::CPow ( int parameters ) : COperator ( parameters ) {}

CValue CPow::evaluate ( const CSpreadsheet & spreadsheet, CValue lhs, CValue rhs ) const {
    if ( std::holds_alternative<double> ( lhs ) && std::holds_alternative<double> ( rhs ) ) {
        return std::pow ( std::get<double> ( lhs ), std::get<double> ( rhs ) );
    } else {
        return std::monostate{};
    }
}

void CPow::print ( std::ostream & os) const {
    os << '^';
}

std::shared_ptr<CItem> CPow::copy () const {
    return std::make_shared<CPow> ( *this );
}