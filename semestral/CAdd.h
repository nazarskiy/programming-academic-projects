#pragma once

#include "COperator.h"

// Concrete classes for arithmetic operators
class CAdd : public COperator {
public:
    CAdd ( int parameters );
    CValue evaluate ( const CSpreadsheet & spreadsheet, CValue lhs, CValue rhs ) const override;
    void print ( std::ostream & os ) const override;
    std::shared_ptr<CItem> copy () const override;
};