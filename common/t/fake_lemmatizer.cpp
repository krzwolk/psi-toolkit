#include "fake_lemmatizer.hpp"

FakeLemmatizer::FakeLemmatizer() {
}

void FakeLemmatizer::lemmatize(const std::string token,
                               AnnotationItemManager& annotationItemManager,
                               LemmatizerOutputIterator& outputIterator) {
    if (token == "prowokacjami") {
        outputIterator.addLemma("prowokacja");

        AnnotationItem lexeme("R", "prowokacja_R");
        annotationItemManager.setValue(lexeme, "flags", "4");
        outputIterator.addLexeme(lexeme);

        AnnotationItem form = lexeme;
        annotationItemManager.setValue(form, "morpho", "ŻNM");
        outputIterator.addForm(form);
    }
}

std::string FakeLemmatizer::getName() {
    return "lex-fake";
}

std::list<std::string> FakeLemmatizer::getLayerTags() {
    std::list<std::string> layerTags;

    layerTags.push_back("lex-fake");
    layerTags.push_back("lexdb-tagset");

    return layerTags;
}