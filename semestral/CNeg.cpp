#pragma once

#include "CNeg.h"

CNeg::CNeg ( int parameters ) : COperator ( parameters ) {}

CValue CNeg::evaluate ( const CSpreadsheet & spreadsheet, CValue lhs, CValue rhs ) const {
    if ( std::holds_alternative<double> ( lhs ) ) {
        return - ( std::get<double> ( lhs ) );
    } else {
        return std::monostate{};
    }
}

void CNeg::print ( std::ostream & os ) const {
    os << '!';
}

std::shared_ptr<CItem> CNeg::copy () const {
    return std::make_shared<CNeg> ( *this );
}