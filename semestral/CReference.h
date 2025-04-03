#pragma once

#include "CItem.h"

// Concrete class for cell references
class CReference : public CItem {
public:
    CReference ( const std::string & value );
    CValue evaluate ( const CSpreadsheet & spreadsheet, CValue lhs, CValue rhs ) const override;
    bool is_operator () const override;
    // Method to get the position of the reference
    const CPos & get_cpos () const;
    // Method to move the reference by a specified offset in rows
    void move_row ( int offset );
    // Method to move the reference by a specified offset in columns
    void move_column ( int offset );
    void mark ( std::ostream & os ) const override;
    void print ( std::ostream & os ) const override;
    std::shared_ptr<CItem> copy () const override;
private:
    CPos m_value; // Position of the reference
};
