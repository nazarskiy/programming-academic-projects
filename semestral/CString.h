#pragma once

#include "CItem.h"

// Concrete class for string values
class CString : public CItem {
public:
    CString ( std::string value );
    CValue evaluate ( const CSpreadsheet & spreadsheet, CValue lhs, CValue rhs ) const override;
    bool is_operator () const override;
    void mark ( std::ostream & os ) const override;
    void print ( std::ostream & os ) const override;
    std::shared_ptr<CItem> copy () const override;
private:
    std::string m_value;
};