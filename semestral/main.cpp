#ifndef __PROGTEST__

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cctype>
#include <climits>
#include <cfloat>
#include <cassert>
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <string>
#include <array>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <stack>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <functional>
#include <iterator>
#include <stdexcept>
#include <variant>
#include <optional>
#include <compare>
#include <charconv>
#include <span>
#include <utility>
#include "expression.h"

using namespace std::literals;
using CValue = std::variant<std::monostate, double, std::string>;

constexpr unsigned SPREADSHEET_CYCLIC_DEPS = 0x01;
constexpr unsigned SPREADSHEET_FUNCTIONS = 0x02;
constexpr unsigned SPREADSHEET_FILE_IO = 0x04;
constexpr unsigned SPREADSHEET_SPEED = 0x08;
constexpr unsigned SPREADSHEET_PARSER = 0x10;
#endif /* __PROGTEST__ */

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

// CItem forward declaration
class CItem;

class CSpreadsheet {
public:
    static unsigned capabilities() {
        return SPREADSHEET_CYCLIC_DEPS | SPREADSHEET_FILE_IO | SPREADSHEET_SPEED;
    }

    CSpreadsheet() = default;
    CSpreadsheet ( const CSpreadsheet & other );

    bool load(std::istream &is);

    bool save(std::ostream &os) const;

    bool setCell(CPos pos,
                 std::string contents);

    CValue getValue(CPos pos) const;

    void copyRect(CPos dst,
                  CPos src,
                  int w = 1,
                  int h = 1);
private:
    // Data structure to store the spreadsheet table
    std::map<CPos, std::vector<std::shared_ptr<CItem>>> m_table;
};
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

int CItem::get_parameters () const {
    return m_parameters;
}

// Concrete class for double values
class CDouble : public CItem {
public:
    CDouble ( double value );
    CValue evaluate ( const CSpreadsheet & spreadsheet, CValue lhs, CValue rhs ) const override;
    bool is_operator () const override;
    void mark ( std::ostream & os ) const override;
    void print ( std::ostream & os ) const override;
    std::shared_ptr<CItem> copy () const override;
private:
    double m_value;
};

CDouble::CDouble ( double value ) : m_value ( value ) {}

CValue CDouble::evaluate ( const CSpreadsheet & spreadsheet, CValue lhs, CValue rhs ) const {
    return m_value;
}

bool CDouble::is_operator () const {
    return false;
}

void CDouble::mark ( std::ostream & os ) const {
    os << "double:";
}

void CDouble::print ( std::ostream & os ) const {
    os << m_value;
}

std::shared_ptr<CItem> CDouble::copy () const {
    return std::make_shared<CDouble> ( *this );
}

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

CString::CString ( std::string value ) : m_value ( std::move ( value ) ) {}

CValue CString::evaluate ( const CSpreadsheet & spreadsheet, CValue lhs, CValue rhs ) const {
    return m_value;
}

bool CString::is_operator () const {
    return false;
}

void CString::mark ( std::ostream & os ) const {
    os << "string:";
}

void CString::print ( std::ostream & os ) const {
    os << m_value;
}

std::shared_ptr<CItem> CString::copy () const {
    return std::make_shared<CString> ( *this );
}

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

CReference::CReference ( const std::string & value ) : m_value ( value ) {}
// Method to evaluate the reference by fetching its value from the spreadsheet
CValue CReference::evaluate ( const CSpreadsheet & spreadsheet, CValue lhs, CValue rhs ) const {
    return spreadsheet.getValue ( m_value );
}

bool CReference::is_operator () const {
    return false;
}

const CPos & CReference::get_cpos () const {
    return m_value;
}

void CReference::move_row ( int offset ) {
    m_value.set_row ( m_value.get_row() - offset );
}

void CReference::move_column ( int offset ) {
    m_value.set_column ( m_value.get_column_numerical() - offset );
}

void CReference::mark ( std::ostream & os ) const {
    os << "reference:";
}

void CReference::print ( std::ostream & os) const {
    m_value.print ( os );
}

std::shared_ptr<CItem> CReference::copy () const {
    return std::make_shared<CReference> ( *this );
}

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

COperator::COperator ( int parameters ) : m_parameters ( parameters ) {}

bool COperator::is_operator () const {
    return true;
}

int COperator::get_parameters () const {
    return m_parameters;
}
// Method to mark the object as an operator when printing
void COperator::mark ( std::ostream & os ) const {
    os << "operator:";
}

// Concrete classes for arithmetic operators
class CAdd : public COperator {
public:
    CAdd ( int parameters );
    CValue evaluate ( const CSpreadsheet & spreadsheet, CValue lhs, CValue rhs ) const override;
    void print ( std::ostream & os ) const override;
    std::shared_ptr<CItem> copy () const override;
};

CAdd::CAdd ( int parameters ) : COperator ( parameters ) {}

CValue CAdd::evaluate ( const CSpreadsheet & spreadsheet, CValue lhs, CValue rhs ) const {
    // Check the types of the operands and perform addition accordingly
    if ( std::holds_alternative<double> ( lhs ) && std::holds_alternative<double> ( rhs ) ) {
        return std::get<double> ( lhs ) + std::get<double> ( rhs );
    } else if ( std::holds_alternative<std::monostate> ( lhs ) || std::holds_alternative<std::monostate> ( rhs ) ) {
        return std::monostate{};
    } else if ( std::holds_alternative<double> ( lhs ) ) {
        return std::to_string ( std::get<double> ( lhs ) ) + std::get<std::string> ( rhs );
    } else if ( std::holds_alternative<double> ( rhs ) ) {
        return std::get<std::string> ( lhs ) + std::to_string ( std::get<double> ( rhs ) );
    } else if ( std::holds_alternative<std::string> ( lhs ) && std::holds_alternative<std::string> ( rhs ) ) {
        return std::get<std::string> ( lhs )  +  std::get<std::string> ( rhs  );
    }
    return std::monostate{};
}

void CAdd::print ( std::ostream & os) const {
    os << '+';
}

std::shared_ptr<CItem> CAdd::copy () const {
    return std::make_shared<CAdd> ( *this );
}

class CSub : public COperator {
public:
    CSub ( int parameters );
    CValue evaluate ( const CSpreadsheet & spreadsheet, CValue lhs, CValue rhs ) const override;
    void print ( std::ostream & os ) const override;
    std::shared_ptr<CItem> copy () const override;
};

CSub::CSub ( int parameters ) : COperator ( parameters ) {}

CValue CSub::evaluate ( const CSpreadsheet & spreadsheet, CValue lhs, CValue rhs ) const {
    if ( std::holds_alternative<double> ( lhs ) && std::holds_alternative<double> ( rhs ) ) {
        return std::get<double> ( lhs ) - std::get<double> ( rhs );
    } else {
        return std::monostate{};
    }
}

void CSub::print ( std::ostream & os ) const {
    os << "-";
}

std::shared_ptr<CItem> CSub::copy () const {
    return std::make_shared<CSub> ( *this );
}

class CMul : public COperator {
public:
    CMul ( int parameters );
    CValue evaluate ( const CSpreadsheet & spreadsheet, CValue lhs, CValue rhs ) const override;
    void print ( std::ostream & os ) const override;
    std::shared_ptr<CItem> copy () const override;
};

CMul::CMul ( int parameters ) : COperator ( parameters ) {}

CValue CMul::evaluate ( const CSpreadsheet & spreadsheet, CValue lhs, CValue rhs ) const {
    if ( std::holds_alternative<double> ( lhs ) && std::holds_alternative<double> ( rhs ) ) {
        return std::get<double> ( lhs ) * std::get<double> ( rhs );
    } else {
        return std::monostate{};
    }
}

void CMul::print ( std::ostream & os ) const {
    os << '*';
}

std::shared_ptr<CItem> CMul::copy () const {
    return std::make_shared<CMul> ( *this );
}

class CDiv : public COperator {
public:
    CDiv ( int parameters );
    CValue evaluate ( const CSpreadsheet & spreadsheet, CValue lhs, CValue rhs ) const override;
    void print ( std::ostream & os ) const override;
    std::shared_ptr<CItem> copy () const override;
};

CDiv::CDiv ( int parameters ) : COperator ( parameters ) {}

CValue CDiv::evaluate ( const CSpreadsheet & spreadsheet, CValue lhs, CValue rhs ) const {
    if ( std::holds_alternative<double> ( lhs ) && std::holds_alternative<double> ( rhs ) ) {
        if ( std::get<double> ( rhs ) == 0 ) {
            return std::monostate();
        }
        return std::get<double> ( lhs ) / std::get<double> ( rhs );
    } else {
        return std::monostate{};
    }
}

void CDiv::print ( std::ostream & os ) const {
    os << '/';
}

std::shared_ptr<CItem> CDiv::copy () const {
    return std::make_shared<CDiv> ( *this );
}

class CPow : public COperator {
public:
    CPow ( int parameters );
    CValue evaluate ( const CSpreadsheet & spreadsheet, CValue lhs, CValue rhs ) const override;
    void print ( std::ostream & os ) const override;
    std::shared_ptr<CItem> copy () const override;
};

CPow::CPow ( int parameters ) : COperator ( parameters ) {}

CValue CPow::evaluate ( const CSpreadsheet & spreadsheet, CValue lhs, CValue rhs ) const {
    if ( std::holds_alternative<double> ( lhs ) && std::holds_alternative<double> ( rhs ) ) {
        return std::pow ( std::get<double> ( lhs ), std::get<double> ( rhs ) );
    } else {
        return std::monostate{};
    }
}

void CPow::print ( std::ostream & os) const {
    os << '^';
}

std::shared_ptr<CItem> CPow::copy () const {
    return std::make_shared<CPow> ( *this );
}

// Concrete class for negation operator
class CNeg : public COperator {
public:
    CNeg ( int parameters );
    CValue evaluate ( const CSpreadsheet & spreadsheet, CValue lhs, CValue rhs ) const override;
    void print ( std::ostream & os ) const override;
    std::shared_ptr<CItem> copy () const override;
};

CNeg::CNeg ( int parameters ) : COperator ( parameters ) {}

CValue CNeg::evaluate ( const CSpreadsheet & spreadsheet, CValue lhs, CValue rhs ) const {
    if ( std::holds_alternative<double> ( lhs ) ) {
        return - ( std::get<double> ( lhs ) );
    } else {
        return std::monostate{};
    }
}

void CNeg::print ( std::ostream & os ) const {
    os << '!';
}

std::shared_ptr<CItem> CNeg::copy () const {
    return std::make_shared<CNeg> ( *this );
}

// Concrete classes for relation operators
class CEq : public COperator {
public:
    CEq ( int parameters );
    CValue evaluate ( const CSpreadsheet & spreadsheet, CValue lhs, CValue rhs ) const override;
    void print ( std::ostream & os ) const override;
    std::shared_ptr<CItem> copy () const override;
};

CEq::CEq ( int parameters ) : COperator ( parameters ) {}

CValue CEq::evaluate ( const CSpreadsheet & spreadsheet, CValue lhs, CValue rhs ) const {
    if ( std::holds_alternative<double> ( lhs ) && std::holds_alternative<double> ( rhs ) ) {
        return 1.0 * ( std::get<double> ( lhs ) == std::get<double> ( rhs ) );
    } else if ( std::holds_alternative<std::string> ( lhs ) && std::holds_alternative<std::string> ( rhs ) ) {
        return 1.0 * ( std::get<std::string> ( lhs ) == std::get<std::string> ( rhs ) );
    } else {
        return std::monostate{};
    }
}

void CEq::print ( std::ostream & os ) const {
    os << "==";
}

std::shared_ptr<CItem> CEq::copy () const {
    return std::make_shared<CEq> ( *this );
}

class CNe : public COperator {
public:
    CNe ( int parameters );
    CValue evaluate ( const CSpreadsheet & spreadsheet, CValue lhs, CValue rhs ) const override;
    void print ( std::ostream & os ) const override;
    std::shared_ptr<CItem> copy () const override;
};

CNe::CNe ( int parameters ) : COperator ( parameters ) {}

CValue CNe::evaluate ( const CSpreadsheet & spreadsheet, CValue lhs, CValue rhs ) const {
    if ( std::holds_alternative<double> ( lhs ) && std::holds_alternative<double> ( rhs ) ) {
        return 1.0 * ( std::get<double> ( lhs ) != std::get<double> ( rhs ) );
    } else if ( std::holds_alternative<std::string> ( lhs ) && std::holds_alternative<std::string> ( rhs ) ) {
        return 1.0 * ( std::get<std::string> ( lhs ) != std::get<std::string> ( rhs ) );
    } else {
        return std::monostate{};
    }
}

void CNe::print ( std::ostream & os ) const {
    os << "!=";
}

std::shared_ptr<CItem> CNe::copy () const {
    return std::make_shared<CNe> ( *this );
}

class CLt : public COperator {
public:
    CLt ( int parameters );
    CValue evaluate ( const CSpreadsheet & spreadsheet, CValue lhs, CValue rhs ) const override;
    void print ( std::ostream & os ) const override;
    std::shared_ptr<CItem> copy () const override;
};

CLt::CLt ( int parameters ) : COperator ( parameters ) {}

CValue CLt::evaluate ( const CSpreadsheet & spreadsheet, CValue lhs, CValue rhs ) const {
    if ( std::holds_alternative<double> ( lhs ) && std::holds_alternative<double> ( rhs ) ) {
        return 1.0 * ( std::get<double> ( lhs ) < std::get<double> ( rhs ) );
    } else if ( std::holds_alternative<std::string> ( lhs ) && std::holds_alternative<std::string> ( rhs ) ) {
        return 1.0 * ( std::get<std::string> ( lhs ) < std::get<std::string> ( rhs ) );
    } else {
        return std::monostate{};
    }
}

void CLt::print ( std::ostream & os ) const {
    os << '<';
}

std::shared_ptr<CItem> CLt::copy () const {
    return std::make_shared<CLt> ( *this );
}

class CLe : public COperator {
public:
    CLe ( int parameters );
    CValue evaluate ( const CSpreadsheet & spreadsheet, CValue lhs, CValue rhs ) const override;
    void print ( std::ostream & os ) const override;
    std::shared_ptr<CItem> copy () const override;
};

CLe::CLe ( int parameters ) : COperator ( parameters ) {}

CValue CLe::evaluate ( const CSpreadsheet & spreadsheet, CValue lhs, CValue rhs ) const {
    if ( std::holds_alternative<double> ( lhs ) && std::holds_alternative<double> ( rhs ) ) {
        return 1.0 * ( std::get<double> ( lhs ) <= std::get<double> ( rhs ) );
    } else if ( std::holds_alternative<std::string> ( lhs ) && std::holds_alternative<std::string> ( rhs ) ) {
        return 1.0 * ( std::get<std::string> ( lhs ) <= std::get<std::string> ( rhs ) );
    } else {
        return std::monostate{};
    }
}

void CLe::print ( std::ostream & os ) const {
    os << "<=";
}

std::shared_ptr<CItem> CLe::copy () const {
    return std::make_shared<CLe> ( *this );
}

class CGt : public COperator {
public:
    CGt ( int parameters );
    CValue evaluate ( const CSpreadsheet & spreadsheet, CValue lhs, CValue rhs ) const override;
    void print ( std::ostream & os ) const override;
    std::shared_ptr<CItem> copy () const override;
};

CGt::CGt ( int parameters ) : COperator ( parameters ) {}

CValue CGt::evaluate ( const CSpreadsheet & spreadsheet, CValue lhs, CValue rhs ) const {
    if ( std::holds_alternative<double> ( lhs ) && std::holds_alternative<double> ( rhs ) ) {
        return 1.0 * ( std::get<double> ( lhs ) > std::get<double> ( rhs ) );
    } else if ( std::holds_alternative<std::string> ( lhs ) && std::holds_alternative<std::string> ( rhs ) ) {
        return 1.0 * ( std::get<std::string> ( lhs ) > std::get<std::string> ( rhs ) );
    } else {
        return std::monostate{};
    }
}

void CGt::print ( std::ostream & os ) const {
    os << '>';
}

std::shared_ptr<CItem> CGt::copy () const {
    return std::make_shared<CGt> ( *this );
}

class CGe : public COperator {
public:
    CGe ( int parameters );
    CValue evaluate ( const CSpreadsheet & spreadsheet, CValue lhs, CValue rhs ) const override;
    void print ( std::ostream & os ) const override;
    std::shared_ptr<CItem> copy () const override;
};

CGe::CGe ( int parameters ) : COperator ( parameters ) {}

CValue CGe::evaluate ( const CSpreadsheet & spreadsheet, CValue lhs, CValue rhs ) const {
    if ( std::holds_alternative<double> ( lhs ) && std::holds_alternative<double> ( rhs ) ) {
        return 1.0 * ( std::get<double> ( lhs ) >= std::get<double> ( rhs ) );
    } else if ( std::holds_alternative<std::string> ( lhs ) && std::holds_alternative<std::string> ( rhs ) ) {
        return 1.0 * ( std::get<std::string> ( lhs ) >= std::get<std::string> ( rhs ) );
    } else {
        return std::monostate{};
    }
}

void CGe::print ( std::ostream & os ) const {
    os << ">=";
}

std::shared_ptr<CItem> CGe::copy () const {
    return std::make_shared<CGe> ( *this );
}

class CMyBuilder : public CExprBuilder {
public:
    // Method to get the constructed expression data
    const std::vector<std::shared_ptr<CItem>> & get_data () const;
private:
    void opAdd () override;

    void opSub () override;

    void opMul () override;

    void opDiv () override;

    void opPow () override;

    void opNeg () override;

    void opEq () override;

    void opNe () override;

    void opLt () override;

    void opLe () override;

    void opGt () override;

    void opGe () override;

    void valNumber ( double val ) override;

    void valString ( std::string val ) override;

    void valReference ( std::string val ) override;

    void valRange ( std::string val ) override;

    void funcCall ( std::string fnName, int paramCount ) override;
    // Vector to store the constructed expression
    std::vector<std::shared_ptr<CItem>> m_expression;
};

const std::vector<std::shared_ptr<CItem>> & CMyBuilder::get_data () const {
    return m_expression;
}

// These methods add various types of operators and operands to the expression vector
void CMyBuilder::opAdd () {
    std::shared_ptr<CItem> new_item = std::make_shared<CAdd> ( 2 );
    m_expression.push_back ( new_item );
}

void CMyBuilder::opSub () {
    std::shared_ptr<CItem> new_item = std::make_shared<CSub> ( 2 );
    m_expression.push_back ( new_item );
}

void CMyBuilder::opMul ()  {
    std::shared_ptr<CItem> new_item = std::make_shared<CMul> ( 2 );
    m_expression.push_back ( new_item );
}

void CMyBuilder::opDiv ()  {
    std::shared_ptr<CItem> new_item = std::make_shared<CDiv> ( 2 );
    m_expression.push_back ( new_item );
}

void CMyBuilder::opPow ()  {
    std::shared_ptr<CItem> new_item = std::make_shared<CPow> ( 2 );
    m_expression.push_back ( new_item );
}

void CMyBuilder::opNeg ()  {
    std::shared_ptr<CItem> new_item = std::make_shared<CNeg> ( 1 );
    m_expression.push_back ( new_item );
}

void CMyBuilder::opEq () {
    std::shared_ptr<CItem> new_item = std::make_shared<CEq> ( 2 );
    m_expression.push_back ( new_item );
}

void CMyBuilder::opNe ()  {
    std::shared_ptr<CItem> new_item = std::make_shared<CNe> ( 2 );
    m_expression.push_back ( new_item );
}

void CMyBuilder::opLt()  {
    std::shared_ptr<CItem> new_item = std::make_shared<CLt> ( 2 );
    m_expression.push_back ( new_item );
}

void CMyBuilder::opLe ()  {
    std::shared_ptr<CItem> new_item = std::make_shared<CLe> ( 2 );
    m_expression.push_back ( new_item );
}

void CMyBuilder::opGt ()  {
    std::shared_ptr<CItem> new_item = std::make_shared<CGt> ( 2 );
    m_expression.push_back ( new_item );
}

void CMyBuilder::opGe ()  {
    std::shared_ptr<CItem> new_item = std::make_shared<CGe> ( 2 );
    m_expression.push_back ( new_item );
}

void CMyBuilder::valNumber ( double val )  {
    std::shared_ptr<CItem> new_item = std::make_shared<CDouble> ( val );
    m_expression.push_back ( new_item );
}

void CMyBuilder::valString ( std::string val )  {
    std::shared_ptr<CItem> new_item = std::make_shared<CString> ( val );
    m_expression.push_back ( new_item );
}

void CMyBuilder::valReference ( std::string val )  {
    std::shared_ptr<CItem> new_item = std::make_shared<CReference> ( val );
    m_expression.push_back ( new_item );
}

void CMyBuilder::valRange ( std::string val ) {}

void CMyBuilder::funcCall ( std::string fnName, int paramCount ) {}

// Copy constructor for CSpreadsheet class. Making a deep copy of the table
CSpreadsheet::CSpreadsheet ( const CSpreadsheet & other ) {
    for ( const auto & it : other.m_table ) {
        for ( const auto & item : it.second ) {
            m_table[it.first].push_back ( item->copy() );
        }
    }
}

bool CSpreadsheet::setCell ( CPos pos, std::string contents ) {
    try {
        // Create a builder object to parse the expression
        CMyBuilder builder;
        parseExpression ( contents, builder );

        m_table[pos].clear();
        // Assign the new contents obtained from the builder
        m_table[pos] = builder.get_data();

        return true;
    } catch ( const std::exception & exception) {
        // If an exception occurs during parsing, return false
        return false;
    }
}
// Vector to keep track of visited positions to detect cyclic dependencies
std::vector<CPos> visited;
CValue CSpreadsheet::getValue ( CPos pos ) const {
    CValue res;

    // Check for cyclic dependencies by iterating through visited positions
    for ( auto & it : visited ) {
        // If the current position matches a visited position, return monostate to indicate cyclic dependency
        if ( it.get_column_numerical() == pos.get_column_numerical() && it.get_row() == pos.get_row() ) {
            return std::monostate{};
        }
    }
    // Mark the current position as visited
    visited.push_back ( pos );

    const auto & it = m_table.find ( pos );

    if ( it == m_table.end() ) {
        visited.pop_back();
        return std::monostate{};
    }

    if ( it->second.size() == 1 ) {
        res = it->second[0]->evaluate ( *this, 0.0, 0.0 );
        visited.pop_back();
        return res;
    }
    // Initialize a deque to store items for expression evaluation
    std::deque<std::shared_ptr<CItem>> m_expression;

    for ( size_t i = 0; i < it->second.size(); i++ ) {
        // If the item is not an operator, push it to the expression deque
        if ( !it->second[i]->is_operator() ) {
            m_expression.push_back ( it->second[i] );
        } else {
            CValue tmp;
            // If the operator has one parameter, evaluate it with the last item in the expression deque
            if ( it->second[i]->get_parameters() == 1 ) {
                tmp = it->second[i]->evaluate ( *this, m_expression.back()->evaluate ( *this, 0.0, 0.0 ), 0.0 );
                m_expression.pop_back();
                // If the operator has two parameters, evaluate it with the last two items in the expression deque
            } else if ( it->second[i]->get_parameters() == 2 ) {
                tmp = it->second[i]->evaluate ( *this, m_expression[m_expression.size() - 2]->evaluate ( *this, 0.0, 0.0 ),
                                                       m_expression.back()->evaluate ( *this, 0.0, 0.0 ) );
                m_expression.pop_back();
                m_expression.pop_back();
            }
            // Convert the result to a corresponding item and push it to the expression deque
            if ( std::holds_alternative<double> ( tmp ) ) {
                m_expression.push_back ( std::make_shared<CDouble> ( std::get<double> ( tmp ) ) );
            } else if ( std::holds_alternative<std::string> ( tmp ) ) {
                m_expression.push_back ( std::make_shared<CString> ( std::get<std::string> ( tmp ) ) );
            } else {
                visited.pop_back();
                return std::monostate{};
            }
        }
    }
    // If the expression deque is not empty, evaluate the remaining item and return the result
    if ( !m_expression.empty() ) {
        res = m_expression[0]->evaluate ( *this, 0.0, 0.0 );
    }

    visited.pop_back();
    
    return res;
}

void CSpreadsheet::copyRect ( CPos dst, CPos src, int w, int h ) {
    // Temporary map to hold the copied data
    std::map<CPos,std::vector<std::shared_ptr<CItem>>> tmp;
    // Calculate the offset between source and destination positions
    int offset_x = src.get_column_numerical() - dst.get_column_numerical();
    int offset_y = src.get_row() - dst.get_row();
    // Iterate over each cell in the rectangular region
    for ( int i = 0; i < w; i++ ) {
        for ( int j = 0; j < h; j++ ) {
            // Calculate the current source and destination positions
            CPos cur_src_pos ( src.get_row() + j, src.get_column_numerical() + i );
            CPos cur_dst_pos ( dst.get_row() + j, dst.get_column_numerical() + i );
            
            const auto & it = m_table.find ( cur_src_pos );
            
            std::vector<std::shared_ptr<CItem>> expression;
            
            if ( it == m_table.end() ) {
                tmp[cur_dst_pos] = expression;
                continue;
            }
            
            for ( const auto & to_copy : it->second ) {
                expression.push_back ( to_copy->copy() );
            }
            // Adjust cell references in the copied expression based on the offset
            for ( const auto & item : expression ) {
                auto CReference_item = std::dynamic_pointer_cast<CReference> ( item );
                if ( CReference_item ) {
                    if ( !CReference_item->get_cpos().get_reference_column() && CReference_item->get_cpos().get_reference_row() ) {
                        CReference_item->move_column ( offset_x );
                    } else if ( CReference_item->get_cpos().get_reference_column() && !CReference_item->get_cpos().get_reference_row() ) {
                        CReference_item->move_row ( offset_y );
                    } else if ( !CReference_item->get_cpos().get_reference_column() && !CReference_item->get_cpos().get_reference_row() ) {
                        CReference_item->move_column ( offset_x );
                        CReference_item->move_row ( offset_y );
                    }
                }
            }
            // Store the expression vector in the temporary map at the current destination position
            tmp[cur_dst_pos] = expression;
        }
    }
    // Copy data from the temporary map to the main table
    for ( const auto & it : tmp ) {
        m_table[it.first] = it.second;
    }
}

bool CSpreadsheet::save ( std::ostream & os ) const {
    for ( const auto & it : m_table ) {
        // Print the cell position
        it.first.print ( os );
        // Start cell data block
        os << "{|\n";
        // Get an iterator to the last item in the cell data vector
        auto last_item = it.second.end();
        --last_item;
        // Iterate over each item in the cell data
        for ( auto item = it.second.begin(); item != it.second.end(); item++ ) {
            // Mark the item type and print its value
            (*item)->mark(os);
            (*item)->print(os);
            
            if ( ! ( item == last_item ) ) {
                os << "\n";
            }
        }
        // End cell data block
        os << "\n|}\n";
    }
    
    return true;
}

bool CSpreadsheet::load ( std::istream & is ) {
    std::string line;
    
    while ( std::getline ( is, line ) ) {
        // Find the position of "{|", indicating the start of a cell
        size_t brace_position = line.find ( "{|" );
        // If "{|" is not found or it's at the end of the line, return false
        if ( brace_position == std::string::npos || brace_position + 2 > line.size() ) {
            return false;
        }
        // Extract the cell position from the line
        std::string cell_position = line.substr ( 0, brace_position );
        CPos cellPos ( cell_position );
        // Read lines until "|}" is encountered, storing cell data in cell_expression
        std::string cell_expression;
        while ( std::getline ( is, line ) && line != "|}" ) {
            cell_expression += line + '\n';
        }
        
        if ( !cell_expression.empty() && cell_expression.back() == '\n' ) {
            cell_expression.pop_back();
        }
        
        std::istringstream expression_steam ( cell_expression );
        std::string citem;
        m_table[cellPos].clear();

        while ( std::getline ( expression_steam, line ) ) {
            // Find the position of ':' to separate item type and value
            size_t colonPos = line.find ( ':' );
            // If ':' is not found, or it's at the end of the line, return false
            if ( colonPos == std::string::npos || colonPos + 1 > line.size() ) {
                return false;
            }
            // Extract item type and value
            citem = line.substr ( 0, colonPos );
            std::string value = line.substr ( colonPos + 1 );
            // Determine item type and create the corresponding object
            if ( citem == "double" ) {
                std::shared_ptr<CItem> to_add = std::make_shared<CDouble> ( std::stod ( value ) );
                m_table[cellPos].push_back ( to_add );
            } else if ( citem == "string" ) {
                std::shared_ptr<CItem> to_add = std::make_shared<CString> ( value );
                m_table[cellPos].push_back ( to_add );
            } else if ( citem == "reference" ) {
                std::shared_ptr<CItem> to_add = std::make_shared<CReference> ( value );
                m_table[cellPos].push_back ( to_add );
            } else if ( citem == "operator" ) {
                // Determine the operator type and create the corresponding object
                std::shared_ptr<CItem> to_add;
                if ( value == "+" ) {
                    to_add = std::make_shared<CAdd> ( 2 );
                } else if ( value == "-" ) {
                    to_add = std::make_shared<CSub> ( 2 );
                } else if ( value == "*" ) {
                    to_add = std::make_shared<CMul> ( 2 );
                } else if ( value == "/" ) {
                    to_add = std::make_shared<CDiv> ( 2 );
                } else if ( value == "^" ) {
                    to_add = std::make_shared<CPow> ( 2 );
                } else if ( value == "!" ) {
                    to_add = std::make_shared<CNeg> ( 1 );
                } else if ( value == "==" ) {
                    to_add = std::make_shared<CEq> ( 2 );
                } else if ( value == "!=" ) {
                    to_add = std::make_shared<CNe> ( 2 );
                } else if ( value == "<" ) {
                    to_add = std::make_shared<CLt> ( 2 );
                } else if ( value == "<=" ) {
                    to_add = std::make_shared<CLe> ( 2 );
                } else if ( value == ">" ) {
                    to_add = std::make_shared<CGt> ( 2 );
                } else if ( value == ">=" ) {
                    to_add = std::make_shared<CGe> ( 2 );
                } else {
                    return false; // Invalid operator type
                }
                m_table[cellPos].push_back ( to_add );
            } else {
                return false; // Invalid item type
            }
        }
    }
    return true;
}

#ifndef __PROGTEST__

bool valueMatch(const CValue &r,
                const CValue &s) {
    if (r.index() != s.index())
        return false;
    if (r.index() == 0)
        return true;
    if (r.index() == 2)
        return std::get<std::string>(r) == std::get<std::string>(s);
    if (std::isnan(std::get<double>(r)) && std::isnan(std::get<double>(s)))
        return true;
    if (std::isinf(std::get<double>(r)) && std::isinf(std::get<double>(s)))
        return (std::get<double>(r) < 0 && std::get<double>(s) < 0)
               || (std::get<double>(r) > 0 && std::get<double>(s) > 0);
    return fabs(std::get<double>(r) - std::get<double>(s)) <= 1e8 * DBL_EPSILON * fabs(std::get<double>(r));
}

static void test0 () {
    CSpreadsheet test01, test02;
    std::ostringstream oss;
    std::istringstream iss;
    std::string data;

    assert ( test01 . setCell ( CPos ( "A1" ), "1" ) );
    assert ( test01 . setCell ( CPos ( "A2" ), "2" ) );
    assert ( test01 . setCell ( CPos ( "A3" ), "=A1+A4" ) );
    assert ( test01 . setCell ( CPos ( "A4" ), "=A2+A3" ) );
    assert ( valueMatch ( test01 . getValue ( CPos ( "A3" ) ), CValue () ) );

    assert ( test01 . setCell ( CPos ( "A5" ), "=A1+A2+A1" ) );
    assert ( valueMatch ( test01 . getValue ( CPos ( "A5" ) ), CValue ( 4.0 ) ) );


    assert ( test01 . setCell ( CPos ( "A6" ), "raw text" ) );
    assert ( test01 . setCell ( CPos ( "A7" ), "= A6 + A1" ) );
    assert ( valueMatch ( test01 . getValue ( CPos ( "A7" ) ), CValue ( "raw text1.000000" ) ) );

    assert ( test01 . setCell ( CPos ( "B2" ), "1" ) );
    assert ( test01 . setCell ( CPos ( "C2" ), "2" ) );
    assert ( test01 . setCell ( CPos ( "B3" ), "3" ) );
    assert ( test01 . setCell ( CPos ( "C3" ), "4" ) );
    assert ( test01 . setCell ( CPos ( "E2" ), "=B2" ) );
    assert ( test01 . setCell ( CPos ( "F2" ), "=$C2" ) );
    assert ( test01 . setCell ( CPos ( "E3" ), "=B$3" ) );
    assert ( test01 . setCell ( CPos ( "F3" ), "=$C$3" ) );
    assert ( test01 . setCell ( CPos ( "D5" ), "5" ) );
    assert ( test01 . setCell ( CPos ( "C5" ), "6" ) );
    assert ( test01 . setCell ( CPos ( "D3" ), "7" ) );
    test01 . copyRect ( CPos ( "G5" ), CPos ( "E2" ), 2, 2 );
    assert ( valueMatch ( test01 . getValue ( CPos ( "G5" ) ), CValue ( 5.0 ) ) );
    assert ( valueMatch ( test01 . getValue ( CPos ( "H5" ) ), CValue ( 6.0 ) ) );
    assert ( valueMatch ( test01 . getValue ( CPos ( "G6" ) ), CValue ( 7.0 ) ) );
    assert ( valueMatch ( test01 . getValue ( CPos ( "H6" ) ), CValue ( 4.0 ) ) );

    oss . clear ();
    oss . str ( "" );
    assert ( test01 . save ( oss ) );
    data = oss . str ();
    iss . clear ();
    iss . str ( data );
    assert ( test02 . load ( iss ) );
    assert ( valueMatch ( test02 . getValue ( CPos ( "A3" ) ), CValue () ) );
    assert ( valueMatch ( test02 . getValue ( CPos ( "A5" ) ), CValue ( 4.0 ) ) );
    assert ( valueMatch ( test02 . getValue ( CPos ( "A7" ) ), CValue ( "raw text1.000000" ) ) );
    assert ( valueMatch ( test02 . getValue ( CPos ( "G5" ) ), CValue ( 5.0 ) ) );
    assert ( valueMatch ( test02 . getValue ( CPos ( "H5" ) ), CValue ( 6.0 ) ) );
    assert ( valueMatch ( test02 . getValue ( CPos ( "G6" ) ), CValue ( 7.0 ) ) );
    assert ( valueMatch ( test02 . getValue ( CPos ( "H6" ) ), CValue ( 4.0 ) ) );
    oss . clear ();
    oss . str ( "" );
    assert ( test01 . save ( oss ) );
    data = oss . str ();
    for ( size_t i = 0; i < std::min<size_t> ( data . length (), 10 ); i ++ )
        data[i] ^=0x5a;
    iss . clear ();
    iss . str ( data );
    assert ( ! test02 . load ( iss ) );

}

int main() {

    test0();

    CSpreadsheet x0, x1;
    std::ostringstream oss;
    std::istringstream iss;
    std::string data;
    assert ( x0 . setCell ( CPos ( "A1" ), "10" ) );
    assert ( x0 . setCell ( CPos ( "A2" ), "20.5" ) );
    assert ( x0 . setCell ( CPos ( "A3" ), "3e1" ) );
    assert ( x0 . setCell ( CPos ( "A4" ), "=40" ) );
    assert ( x0 . setCell ( CPos ( "A5" ), "=5e+1" ) );
    assert ( x0 . setCell ( CPos ( "A6" ), "raw text with any characters, including a quote \" or a newline\n" ) );
    assert ( x0 . setCell ( CPos ( "A7" ), "=\"quoted string, quotes must be doubled: \"\". Moreover, backslashes are needed for C++.\"" ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "A1" ) ), CValue ( 10.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "A2" ) ), CValue ( 20.5 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "A3" ) ), CValue ( 30.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "A4" ) ), CValue ( 40.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "A5" ) ), CValue ( 50.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "A6" ) ), CValue ( "raw text with any characters, including a quote \" or a newline\n" ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "A7" ) ), CValue ( "quoted string, quotes must be doubled: \". Moreover, backslashes are needed for C++." ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "A8" ) ), CValue() ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "AAAA9999" ) ), CValue() ) );
    assert ( x0 . setCell ( CPos ( "B1" ), "=A1+A2*A3" ) );
    assert ( x0 . setCell ( CPos ( "B2" ), "= -A1 ^ 2 - A2 / 2   " ) );
    assert ( x0 . setCell ( CPos ( "B3" ), "= 2 ^ $A$1" ) );
    assert ( x0 . setCell ( CPos ( "B4" ), "=($A1+A$2)^2" ) );
    assert ( x0 . setCell ( CPos ( "B5" ), "=B1+B2+B3+B4" ) );
    assert ( x0 . setCell ( CPos ( "B6" ), "=B1+B2+B3+B4+B5" ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "B1" ) ), CValue ( 625.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "B2" ) ), CValue ( -110.25 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "B3" ) ), CValue ( 1024.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "B4" ) ), CValue ( 930.25 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "B5" ) ), CValue ( 2469.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "B6" ) ), CValue ( 4938.0 ) ) );
    assert ( x0 . setCell ( CPos ( "A1" ), "12" ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "B1" ) ), CValue ( 627.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "B2" ) ), CValue ( -154.25 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "B3" ) ), CValue ( 4096.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "B4" ) ), CValue ( 1056.25 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "B5" ) ), CValue ( 5625.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "B6" ) ), CValue ( 11250.0 ) ) );
    x1 = x0;
    assert ( x0 . setCell ( CPos ( "A2" ), "100" ) );
    assert ( x1 . setCell ( CPos ( "A2" ), "=A3+A5+A4" ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "B1" ) ), CValue ( 3012.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "B2" ) ), CValue ( -194.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "B3" ) ), CValue ( 4096.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "B4" ) ), CValue ( 12544.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "B5" ) ), CValue ( 19458.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "B6" ) ), CValue ( 38916.0 ) ) );
    assert ( valueMatch ( x1 . getValue ( CPos ( "B1" ) ), CValue ( 3612.0 ) ) );
    assert ( valueMatch ( x1 . getValue ( CPos ( "B2" ) ), CValue ( -204.0 ) ) );
    assert ( valueMatch ( x1 . getValue ( CPos ( "B3" ) ), CValue ( 4096.0 ) ) );
    assert ( valueMatch ( x1 . getValue ( CPos ( "B4" ) ), CValue ( 17424.0 ) ) );
    assert ( valueMatch ( x1 . getValue ( CPos ( "B5" ) ), CValue ( 24928.0 ) ) );
    assert ( valueMatch ( x1 . getValue ( CPos ( "B6" ) ), CValue ( 49856.0 ) ) );
    oss . clear ();
    oss . str ( "" );
    assert ( x0 . save ( oss ) );
    data = oss . str ();
    iss . clear ();
    iss . str ( data );
    assert ( x1 . load ( iss ) );
    assert ( valueMatch ( x1 . getValue ( CPos ( "B1" ) ), CValue ( 3012.0 ) ) );
    assert ( valueMatch ( x1 . getValue ( CPos ( "B2" ) ), CValue ( -194.0 ) ) );
    assert ( valueMatch ( x1 . getValue ( CPos ( "B3" ) ), CValue ( 4096.0 ) ) );
    assert ( valueMatch ( x1 . getValue ( CPos ( "B4" ) ), CValue ( 12544.0 ) ) );
    assert ( valueMatch ( x1 . getValue ( CPos ( "B5" ) ), CValue ( 19458.0 ) ) );
    assert ( valueMatch ( x1 . getValue ( CPos ( "B6" ) ), CValue ( 38916.0 ) ) );
    assert ( x0 . setCell ( CPos ( "A3" ), "4e1" ) );
    assert ( valueMatch ( x1 . getValue ( CPos ( "B1" ) ), CValue ( 3012.0 ) ) );
    assert ( valueMatch ( x1 . getValue ( CPos ( "B2" ) ), CValue ( -194.0 ) ) );
    assert ( valueMatch ( x1 . getValue ( CPos ( "B3" ) ), CValue ( 4096.0 ) ) );
    assert ( valueMatch ( x1 . getValue ( CPos ( "B4" ) ), CValue ( 12544.0 ) ) );
    assert ( valueMatch ( x1 . getValue ( CPos ( "B5" ) ), CValue ( 19458.0 ) ) );
    assert ( valueMatch ( x1 . getValue ( CPos ( "B6" ) ), CValue ( 38916.0 ) ) );
    oss . clear ();
    oss . str ( "" );
    assert ( x0 . save ( oss ) );
    data = oss . str ();
    for ( size_t i = 0; i < std::min<size_t> ( data . length (), 10 ); i ++ )
        data[i] ^=0x5a;
    iss . clear ();
    iss . str ( data );
    assert ( ! x1 . load ( iss ) );
    assert ( x0 . setCell ( CPos ( "D0" ), "10" ) );
    assert ( x0 . setCell ( CPos ( "D1" ), "20" ) );
    assert ( x0 . setCell ( CPos ( "D2" ), "30" ) );
    assert ( x0 . setCell ( CPos ( "D3" ), "40" ) );
    assert ( x0 . setCell ( CPos ( "D4" ), "50" ) );
    assert ( x0 . setCell ( CPos ( "E0" ), "60" ) );
    assert ( x0 . setCell ( CPos ( "E1" ), "70" ) );
    assert ( x0 . setCell ( CPos ( "E2" ), "80" ) );
    assert ( x0 . setCell ( CPos ( "E3" ), "90" ) );
    assert ( x0 . setCell ( CPos ( "E4" ), "100" ) );
    assert ( x0 . setCell ( CPos ( "F10" ), "=D0+5" ) );
    assert ( x0 . setCell ( CPos ( "F11" ), "=$D0+5" ) );
    assert ( x0 . setCell ( CPos ( "F12" ), "=D$0+5" ) );
    assert ( x0 . setCell ( CPos ( "F13" ), "=$D$0+5" ) );
    x0 . copyRect ( CPos ( "G11" ), CPos ( "F10" ), 1, 4 );
    assert ( valueMatch ( x0 . getValue ( CPos ( "F10" ) ), CValue ( 15.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "F11" ) ), CValue ( 15.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "F12" ) ), CValue ( 15.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "F13" ) ), CValue ( 15.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "F14" ) ), CValue() ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "G10" ) ), CValue() ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "G11" ) ), CValue ( 75.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "G12" ) ), CValue ( 25.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "G13" ) ), CValue ( 65.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "G14" ) ), CValue ( 15.0 ) ) );
    x0 . copyRect ( CPos ( "G11" ), CPos ( "F10" ), 2, 4 );
    assert ( valueMatch ( x0 . getValue ( CPos ( "F10" ) ), CValue ( 15.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "F11" ) ), CValue ( 15.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "F12" ) ), CValue ( 15.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "F13" ) ), CValue ( 15.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "F14" ) ), CValue() ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "G10" ) ), CValue() ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "G11" ) ), CValue ( 75.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "G12" ) ), CValue ( 25.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "G13" ) ), CValue ( 65.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "G14" ) ), CValue ( 15.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "H10" ) ), CValue() ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "H11" ) ), CValue() ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "H12" ) ), CValue() ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "H13" ) ), CValue ( 35.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "H14" ) ), CValue() ) );
    assert ( x0 . setCell ( CPos ( "F0" ), "-27" ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "H14" ) ), CValue ( -22.0 ) ) );
    x0 . copyRect ( CPos ( "H12" ), CPos ( "H13" ), 1, 2 );
    assert ( valueMatch ( x0 . getValue ( CPos ( "H12" ) ), CValue ( 25.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "H13" ) ), CValue ( -22.0 ) ) );
    assert ( valueMatch ( x0 . getValue ( CPos ( "H14" ) ), CValue ( -22.0 ) ) );
    return EXIT_SUCCESS;
}

#endif /* __PROGTEST__ */