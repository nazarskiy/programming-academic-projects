#pragma once

#include "COperator.h"

class CLe : public COperator {
public:
    CLe ( int parameters );
    CValue evaluate ( const CSpreadsheet & spreadsheet, CValue lhs, CValue rhs ) const override;
    void print ( std::ostream & os ) const override;
    std::shared_ptr<CItem> copy () const override;
};