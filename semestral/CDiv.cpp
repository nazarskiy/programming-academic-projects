#pragma once

#include "CDiv.h"

CDiv::CDiv ( int parameters ) : COperator ( parameters ) {}

CValue CDiv::evaluate ( const CSpreadsheet & spreadsheet, CValue lhs, CValue rhs ) const {
    if ( std::holds_alternative<double> ( lhs ) && std::holds_alternative<double> ( rhs ) ) {
        if ( std::get<double> ( rhs ) == 0 ) {
            return std::monostate();
        }
        return std::get<double> ( lhs ) / std::get<double> ( rhs );
    } else {
        return std::monostate{};
    }
}

void CDiv::print ( std::ostream & os ) const {
    os << '/';
}

std::shared_ptr<CItem> CDiv::copy () const {
    return std::make_shared<CDiv> ( *this );
}