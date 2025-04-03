
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