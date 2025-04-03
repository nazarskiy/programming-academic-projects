#pragma once

#include "CPos.h"

// Constructor for the cell position from string
CPos::CPos ( std::string_view str ) : m_row ( 0 ), m_column_numerical ( 0 ), m_reference_column ( false ), m_reference_row ( false ) {
    std::string row;
    bool reference_in_string = false;

    for ( const auto & it : str ) {
        if ( it == '$' ) {
            if ( m_column.empty() && row.empty() ) {
                m_reference_column = true;
            } else if ( !m_column.empty() && row.empty() && !reference_in_string ) {
                m_reference_row = true;
                reference_in_string = true;
            } else {
                throw std::invalid_argument ( "invalid cell position" );
            }
        } else if ( ( !std::isalpha ( it ) && m_column.empty() )
                    || ( !std::isalpha ( it ) && !std::isdigit ( it ) ) ) {
            throw std::invalid_argument ( "invalid cell position" );
        } else if ( std::isalpha ( it ) ) {
            if ( !m_column.empty() && !row.empty() ) {
                throw std::invalid_argument ( "invalid cell position" );
            } else {
                m_column.push_back ( it );
                auto c = std::toupper ( it ) - 'A' + 1;
                m_column_numerical *= 26;
                m_column_numerical += c;
            }
        } else if ( std::isdigit ( it ) ) {
            row.push_back ( it );
        }
    }

    m_row = std::stoi ( row );

}
// Constructor for the cell position from row and column numbers
CPos::CPos ( int row, int column ) : m_row ( row ), m_column_numerical ( column ) {
    int t_column_numerical = m_column_numerical;
    while ( t_column_numerical > 0 ) {
        int column_code = t_column_numerical % 26;
        m_column.push_back ( static_cast<char> ( column_code + 'A' - 1 ) );
        t_column_numerical /= 26;
    }

    std::reverse ( m_column.begin(), m_column.end() );
}
// Getter for the row number
int CPos::get_row () const {
    return m_row;
}
// Getter for the column number
int CPos::get_column_numerical () const {
    return m_column_numerical;
}
// Getter for the column reference flag
bool CPos::get_reference_column() const {
    return m_reference_column;
}
// Getter for the row reference flag
bool CPos::get_reference_row() const {
    return m_reference_row;
}
// Setter for the row
void CPos::set_row ( int row )  {
    m_row = row;
}
// Setter for the column number
void CPos::set_column ( int column ) {
    m_column_numerical = column;
    m_column.clear();
    int t_column_numerical = m_column_numerical;
    while ( t_column_numerical > 0 ) {
        int column_code = t_column_numerical % 26;
        m_column.push_back ( static_cast<char> ( column_code + 'A' - 1 ) );
        t_column_numerical /= 26;
    }

    std::reverse ( m_column.begin(), m_column.end() );
}
// Less than operator for comparison
bool CPos::operator < ( const CPos & other ) const {
    if ( m_column_numerical != other.m_column_numerical ) {
        return m_column_numerical < other.m_column_numerical;
    }
    return m_row < other.m_row;
}
// Print method for cell position
void CPos::print ( std::ostream & os ) const {
    if ( m_reference_column ) {
        os << '$';
    }
    os << m_column;
    if ( m_reference_row ) {
        os << '$';
    }
    os << m_row;
}
