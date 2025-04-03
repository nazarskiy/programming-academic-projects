#pragma once

#include "CAdd.h"
#include "CSub.h"
#include "CMul.h"
#include "CDiv.h"
#include "CPow.h"
#include "CNeg.h"
#include "CEq.h"
#include "CNe.h"
#include "CLt.h"
#include "CLe.h"
#include "CGt.h"
#include "CGe.h"
#include "CDouble.h"
#include "CString.h"
#include "CReference.h"

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