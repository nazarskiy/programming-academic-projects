#pragma once

#include "COperator.h"

// Concrete class for negation operator
class CNeg : public COperator {
public:
    CNeg ( int parameters );
    CValue evaluate ( const CSpreadsheet & spreadsheet, CValue lhs, CValue rhs ) const override;
    void print ( std::ostream & os ) const override;
    std::shared_ptr<CItem> copy () const override;
};