#ifndef expression_h_09845924528375
#define expression_h_09845924528375

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
#include <string>

class CExprBuilder {
public:
    virtual void opAdd () = 0;

    virtual void opSub () = 0;

    virtual void opMul () = 0;

    virtual void opDiv () = 0;

    virtual void opPow () = 0;

    virtual void opNeg () = 0;

    virtual void opEq () = 0;

    virtual void opNe () = 0;

    virtual void opLt () = 0;

    virtual void opLe () = 0;

    virtual void opGt () = 0;

    virtual void opGe () = 0;

    virtual void valNumber ( double val ) = 0;

    virtual void valString ( std::string val ) = 0;

    virtual void valReference ( std::string val ) = 0;

    virtual void valRange ( std::string val ) = 0;

    virtual void funcCall ( std::string fnName,
                            int paramCount ) = 0;
};

void parseExpression ( std::string expr,
                       CExprBuilder & builder );

#endif /* expression_h_09845924528375 */
