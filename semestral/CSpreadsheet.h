#pragma once

#include "CPos.h"

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