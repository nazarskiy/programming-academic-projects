#pragma once

#include "CItem.h"

// Abstract base class for operators
class COperator : public CItem {
public:
    COperator ( int parameters );
    bool is_operator () const override;
    int get_parameters () const override;
    void mark ( std::ostream & os ) const override;
protected:
    int m_parameters;
};