#pragma once

#include "CSpreadsheet.h"
#include "CMyBuilder.h"

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
            // If ':' is not found or it's at the end of the line, return false
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