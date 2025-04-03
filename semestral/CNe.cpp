#pragma once

#include "CNe.h"

CNe::CNe ( int parameters ) : COperator ( parameters ) {}

CValue CNe::evaluate ( const CSpreadsheet & spreadsheet, CValue lhs, CValue rhs ) const {
    if ( std::holds_alternative<double> ( lhs ) && std::holds_alternative<double> ( rhs ) ) {
        return 1.0 * ( std::get<double> ( lhs ) != std::get<double> ( rhs ) );
    } else if ( std::holds_alternative<std::string> ( lhs ) && std::holds_alternative<std::string> ( rhs ) ) {
        return 1.0 * ( std::get<std::string> ( lhs ) != std::get<std::string> ( rhs ) );
    } else {
        return std::monostate{};
    }
}

void CNe::print ( std::ostream & os ) const {
    os << "!=";
}

std::shared_ptr<CItem> CNe::copy () const {
    return std::make_shared<CNe> ( *this );
}