#include "tests.hpp"

#include <boost/scoped_ptr.hpp>

#include "psi_quoter.hpp"
#include "formats/bracketing/bracketing-writer/bracketing_quoter.hpp"
#include "formats/utt/utt_quoter.hpp"

BOOST_AUTO_TEST_SUITE( quoter )

BOOST_AUTO_TEST_CASE( psi_quoter ) {
    boost::scoped_ptr<Quoter> quoter(new PsiQuoter());
    BOOST_CHECK_EQUAL(quoter->escape("a"), "a");
    BOOST_CHECK_EQUAL(quoter->unescape("a"), "a");
    BOOST_CHECK_EQUAL(quoter->escape("ą"), "ą");
    BOOST_CHECK_EQUAL(quoter->unescape("ą"), "ą");
    BOOST_CHECK_EQUAL(quoter->escape(" "), "_");
    BOOST_CHECK_EQUAL(quoter->unescape("_"), " ");
    BOOST_CHECK_EQUAL(quoter->escape("\n"), "\\n");
    BOOST_CHECK_EQUAL(quoter->unescape("\\n"), "\n");
    BOOST_CHECK_EQUAL(quoter->escape("\t"), "\\t");
    BOOST_CHECK_EQUAL(quoter->unescape("\\t"), "\t");
    BOOST_CHECK_EQUAL(quoter->escape("\r"), "\\r");
    BOOST_CHECK_EQUAL(quoter->unescape("\\r"), "\r");
    BOOST_CHECK_EQUAL(quoter->escape("*"), "\\*");
    BOOST_CHECK_EQUAL(quoter->unescape("\\*"), "*");
    BOOST_CHECK_EQUAL(quoter->escape("\\"), "\\\\");
    BOOST_CHECK_EQUAL(quoter->unescape("\\\\"), "\\");
    BOOST_CHECK_EQUAL(quoter->escape("#"), "\\#");
    BOOST_CHECK_EQUAL(quoter->unescape("\\#"), "#");
    BOOST_CHECK_EQUAL(quoter->escape("Zażółć gęślą jaźń!"), "Zażółć_gęślą_jaźń!");
    BOOST_CHECK_EQUAL(quoter->unescape("Zażółć_gęślą_jaźń!"), "Zażółć gęślą jaźń!");
}

BOOST_AUTO_TEST_CASE( psi_quoter_illegal_sequence ) {
    boost::scoped_ptr<Quoter> quoter(new PsiQuoter());
    BOOST_CHECK_THROW(
        quoter->unescape("Abcd\\"),
        QuoterException);

    BOOST_CHECK_THROW(
        quoter->unescape("Ab\\cd"),
        QuoterException);
}

BOOST_AUTO_TEST_CASE( utt_quoter ) {
    boost::scoped_ptr<Quoter> quoter(new UTTQuoter());
    BOOST_CHECK_EQUAL(quoter->escape("a"), "a");
    BOOST_CHECK_EQUAL(quoter->unescape("a"), "a");
    BOOST_CHECK_EQUAL(quoter->escape("ą"), "ą");
    BOOST_CHECK_EQUAL(quoter->unescape("ą"), "ą");
    BOOST_CHECK_EQUAL(quoter->escape(" "), "_");
    BOOST_CHECK_EQUAL(quoter->unescape("_"), " ");
    BOOST_CHECK_EQUAL(quoter->escape("\n"), "\\n");
    BOOST_CHECK_EQUAL(quoter->unescape("\\n"), "\n");
    BOOST_CHECK_EQUAL(quoter->escape("\t"), "\\t");
    BOOST_CHECK_EQUAL(quoter->unescape("\\t"), "\t");
    BOOST_CHECK_EQUAL(quoter->escape("\r"), "\\r");
    BOOST_CHECK_EQUAL(quoter->unescape("\\r"), "\r");
    BOOST_CHECK_EQUAL(quoter->escape("*"), "\\*");
    BOOST_CHECK_EQUAL(quoter->unescape("\\*"), "*");
    BOOST_CHECK_EQUAL(quoter->escape("\\"), "\\\\");
    BOOST_CHECK_EQUAL(quoter->unescape("\\\\"), "\\");
    BOOST_CHECK_EQUAL(quoter->escape("#"), "#");
    BOOST_CHECK_EQUAL(quoter->unescape("#"), "#");
    BOOST_CHECK_EQUAL(quoter->escape("Zażółć gęślą jaźń!"), "Zażółć_gęślą_jaźń!");
    BOOST_CHECK_EQUAL(quoter->unescape("Zażółć_gęślą_jaźń!"), "Zażółć gęślą jaźń!");
}

BOOST_AUTO_TEST_CASE( utt_quoter_illegal_sentence ) {
    boost::scoped_ptr<Quoter> quoter(new UTTQuoter());
    BOOST_CHECK_THROW(
        quoter->unescape("Ab#d\\"),
        QuoterException);
    BOOST_CHECK_THROW(
        quoter->unescape("Ab\\#d"),
        QuoterException);
}

BOOST_AUTO_TEST_CASE( bracketing_quoter ) {
    boost::scoped_ptr<Quoter> quoter(new BracketingQuoter());
    BOOST_CHECK_EQUAL(quoter->escape("a"), "a");
    BOOST_CHECK_EQUAL(quoter->unescape("a"), "a");
    BOOST_CHECK_EQUAL(quoter->escape("ą"), "ą");
    BOOST_CHECK_EQUAL(quoter->unescape("ą"), "ą");
    BOOST_CHECK_EQUAL(quoter->escape(" "), " ");
    BOOST_CHECK_EQUAL(quoter->unescape(" "), " ");
    BOOST_CHECK_EQUAL(quoter->escape("_"), "_");
    BOOST_CHECK_EQUAL(quoter->unescape("_"), "_");
    BOOST_CHECK_EQUAL(quoter->escape("\n"), "\\n");
    BOOST_CHECK_EQUAL(quoter->unescape("\\n"), "\n");
    BOOST_CHECK_EQUAL(quoter->escape("\t"), "\\t");
    BOOST_CHECK_EQUAL(quoter->unescape("\\t"), "\t");
    BOOST_CHECK_EQUAL(quoter->escape("\r"), "\\r");
    BOOST_CHECK_EQUAL(quoter->unescape("\\r"), "\r");
    BOOST_CHECK_EQUAL(quoter->escape("*"), "*");
    BOOST_CHECK_EQUAL(quoter->unescape("*"), "*");
    BOOST_CHECK_EQUAL(quoter->escape("\\"), "\\\\");
    BOOST_CHECK_EQUAL(quoter->unescape("\\\\"), "\\");
    BOOST_CHECK_EQUAL(quoter->escape("#"), "#");
    BOOST_CHECK_EQUAL(quoter->unescape("#"), "#");
    BOOST_CHECK_EQUAL(quoter->escape("Zażółć gęślą jaźń!"), "Zażółć gęślą jaźń!");
    BOOST_CHECK_EQUAL(quoter->unescape("Zażółć gęślą jaźń!"), "Zażółć gęślą jaźń!");
}

BOOST_AUTO_TEST_CASE( bracketing_quoter_illegal_sentence ) {
    boost::scoped_ptr<Quoter> quoter(new BracketingQuoter());
    BOOST_CHECK_THROW(
        quoter->unescape("Ab#d\\"),
        QuoterException);
}

BOOST_AUTO_TEST_SUITE_END()
