#pragma once

#include "CAdd.h"

CAdd::CAdd ( int parameters ) : COperator ( parameters ) {}

CValue CAdd::evaluate ( const CSpreadsheet & spreadsheet, CValue lhs, CValue rhs ) const {
    // Check the types of the operands and perform addition accordingly
    if ( std::holds_alternative<double> ( lhs ) && std::holds_alternative<double> ( rhs ) ) {
        return std::get<double> ( lhs ) + std::get<double> ( rhs );
    } else if ( std::holds_alternative<std::monostate> ( lhs ) || std::holds_alternative<std::monostate> ( rhs ) ) {
        return std::monostate{};
    } else if ( std::holds_alternative<double> ( lhs ) ) {
        return std::to_string ( std::get<double> ( lhs ) ) + std::get<std::string> ( rhs );
    } else if ( std::holds_alternative<double> ( rhs ) ) {
        return std::get<std::string> ( lhs ) + std::to_string ( std::get<double> ( rhs ) );
    } else if ( std::holds_alternative<std::string> ( lhs ) && std::holds_alternative<std::string> ( rhs ) ) {
        return std::get<std::string> ( lhs )  +  std::get<std::string> ( rhs  );
    }
    return std::monostate{};
}

void CAdd::print ( std::ostream & os) const {
    os << '+';
}

std::shared_ptr<CItem> CAdd::copy () const {
    return std::make_shared<CAdd> ( *this );
}