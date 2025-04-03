#pragma once

#include "definitions.h"

// Represents a cell position
class CPos {
public:
    // Constructors
    CPos ( std::string_view str );
    CPos ( int row, int column );
    CPos ( const CPos & other ) = default;
    // Getter methods
    int get_row () const;
    int get_column_numerical () const;
    bool get_reference_column () const;
    bool get_reference_row () const;
    // Setter methods
    void set_row ( int row );
    void set_column ( int column );
    // < operator for the forward map comparator
    bool operator < ( const CPos & other ) const;
    // Print method
    void print ( std::ostream & os ) const;
private:
    std::string m_column;     // Column int the string representation
    int m_row;                // Row number
    int m_column_numerical;   // Column in the numeric representation
    bool m_reference_column;  // Flag indicating if the column is a reference
    bool m_reference_row;     // Flag indicating if the row is a reference
};