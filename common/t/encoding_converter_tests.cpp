#include "tests.hpp"
#include "config.hpp"

#include <iostream>
#include <fstream>

#include "encoding_converter.hpp"

std::string getContentOfExampleFile(std::string fileName) {
    std::ifstream file(std::string(ROOT_DIR "common/t/" + fileName).c_str());

    std::string text, line;
    while (std::getline(file, line)) text += line;

    file.close();
    return text;
}

BOOST_AUTO_TEST_SUITE( encoding_converter )

BOOST_AUTO_TEST_CASE( direct_convertions ) {
    EncodingConverter converter;

    std::string input = getContentOfExampleFile("example_iso-8859-2.txt");
    std::string result = converter.convert("ISO-8859-2", "UTF-8", input);

    BOOST_CHECK_EQUAL("Zażółć gęślą jaźń.", result);

    input = getContentOfExampleFile("example_windows-1255.txt");
    result = converter.convert("windows-1255", "UTF-8", input);

    BOOST_CHECK_EQUAL("למההםפשוטלאמדבריםעברי", result);

    input = getContentOfExampleFile("example_windows-1251.txt");
    result = converter.convert("windows-1251", "UTF-8", input);

    BOOST_CHECK_EQUAL("Этот только UTF, да и вообще перевод не корректный", result);
}

BOOST_AUTO_TEST_SUITE_END()