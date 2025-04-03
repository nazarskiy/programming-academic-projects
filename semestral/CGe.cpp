#pragma once

#include "CGe.h"

CGe::CGe ( int parameters ) : COperator ( parameters ) {}

CValue CGe::evaluate ( const CSpreadsheet & spreadsheet, CValue lhs, CValue rhs ) const {
    if ( std::holds_alternative<double> ( lhs ) && std::holds_alternative<double> ( rhs ) ) {
        return 1.0 * ( std::get<double> ( lhs ) >= std::get<double> ( rhs ) );
    } else if ( std::holds_alternative<std::string> ( lhs ) && std::holds_alternative<std::string> ( rhs ) ) {
        return 1.0 * ( std::get<std::string> ( lhs ) >= std::get<std::string> ( rhs ) );
    } else {
        return std::monostate{};
    }
}

void CGe::print ( std::ostream & os ) const {
    os << ">=";
}

std::shared_ptr<CItem> CGe::copy () const {
    return std::make_shared<CGe> ( *this );
}
