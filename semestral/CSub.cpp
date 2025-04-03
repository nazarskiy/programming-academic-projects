#pragma once

#include "CSub.h"

CSub::CSub ( int parameters ) : COperator ( parameters ) {}

CValue CSub::evaluate ( const CSpreadsheet & spreadsheet, CValue lhs, CValue rhs ) const {
    if ( std::holds_alternative<double> ( lhs ) && std::holds_alternative<double> ( rhs ) ) {
        return std::get<double> ( lhs ) - std::get<double> ( rhs );
    } else {
        return std::monostate{};
    }
}

void CSub::print ( std::ostream & os ) const {
    os << "-";
}

std::shared_ptr<CItem> CSub::copy () const {
    return std::make_shared<CSub> ( *this );
}