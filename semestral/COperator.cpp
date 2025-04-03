#pragma once

#include "COperator.h"

COperator::COperator ( int parameters ) : m_parameters ( parameters ) {}

bool COperator::is_operator () const {
    return true;
}

int COperator::get_parameters () const {
    return m_parameters;
}

// Method to mark the object as an operator when printing
void COperator::mark ( std::ostream & os ) const {
    os << "operator:";
}