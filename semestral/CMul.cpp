#pragma once

#include "CMul.h"

CMul::CMul ( int parameters ) : COperator ( parameters ) {}

CValue CMul::evaluate ( const CSpreadsheet & spreadsheet, CValue lhs, CValue rhs ) const {
    if ( std::holds_alternative<double> ( lhs ) && std::holds_alternative<double> ( rhs ) ) {
        return std::get<double> ( lhs ) * std::get<double> ( rhs );
    } else {
        return std::monostate{};
    }
}

void CMul::print ( std::ostream & os ) const {
    os << '*';
}

std::shared_ptr<CItem> CMul::copy () const {
    return std::make_shared<CMul> ( *this );
}