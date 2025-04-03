#pragma once

#include "CSpreadsheet.h"

// Base class for items in cells
class CItem {
public:
    ~CItem() = default;
    // Pure virtual method to evaluate the item
    virtual CValue evaluate ( const CSpreadsheet & spreadsheet, CValue lhs, CValue rhs ) const = 0;
    // Pure virtual method to check if the item is an operator
    virtual bool is_operator () const = 0;
    // Virtual method to get the number of parameters
    virtual int get_parameters () const;
    // Pure virtual method to mark the item
    virtual void mark ( std::ostream & os ) const = 0;
    // Pure virtual method to print the item value
    virtual void print ( std::ostream & os ) const = 0;
    // Pure virtual method to create a copy of the item
    virtual std::shared_ptr<CItem> copy () const = 0;
protected:
    int m_parameters;
};