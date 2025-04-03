#pragma once

#include "COperator.h"

// Concrete classes for relation operators
class CEq : public COperator {
public:
    CEq ( int parameters );
    CValue evaluate ( const CSpreadsheet & spreadsheet, CValue lhs, CValue rhs ) const override;
    void print ( std::ostream & os ) const override;
    std::shared_ptr<CItem> copy () const override;
};