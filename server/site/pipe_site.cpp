#include "pipe_site.hpp"
#include "pipe_runner.hpp"
#include "logging.hpp"
#include "session_manager.hpp"
#include "main_factories_keeper.hpp"

#include <iostream>
#include <sstream>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/predicate.hpp>

const std::string PipeSite::GUESSING_READER = "guessing-reader";

const unsigned int PipeSite::MAX_INPUT_SIZE = 524288;
const std::string PipeSite::TOO_LARGE_INPUT_MSG = "Input is too large. Limit is 512 KBs.";

PipeSite::PipeSite(PsiServer& server, const std::string& pipe, const std::string& text)
    : TemplateSite(server),
    initialText_(text.c_str()),
    initialPipe_(pipe.c_str()),
    inputFromFile_(false),
    fileStorage_(std::string(psiServer_.websiteRoot)),
    encodingConverter_("UTF-8")
{
    initialOutput_ = runPipe_(initialPipe_, initialText_, false),
    registerIncludesAndActions_();
}

void PipeSite::registerIncludesAndActions_() {
    psiServer_.registerIncludeCode(
        "pipe_site_input_text", boost::bind(&PipeSite::inputText, this));

    psiServer_.registerIncludeCode(
        "pipe_site_pipe_text", boost::bind(&PipeSite::pipeText, this));

    psiServer_.registerActionCode(
        "pipe", boost::bind(&PipeSite::actionPipe, this));

    psiServer_.registerIncludeCode(
        "pipe_site_output_text", boost::bind(&PipeSite::outputText, this));

    psiServer_.registerIncludeCode(
        "pipe_site_options", boost::bind(&PipeSite::hiddenOptions, this));
}

char * PipeSite::inputText() {
    std::string input = getOrSetDefaultData_("input-text", initialText_);
    return stringToChar(input);
}

char * PipeSite::pipeText() {
    std::string pipe = getOrSetDefaultData_("pipe-text", initialPipe_);
    return stringToChar(encodeHTML_(pipe));
}

char * PipeSite::outputText() {
    std::string output = getOrSetDefaultData_("output-text", initialOutput_);
    return stringToChar(generateOutput_(output));
}

std::string PipeSite::getInput_() {
    std::string input = psiServer_.session()->getData("input-text");
    std::string isFile = psiServer_.session()->getData("radio-file");

    if (isFile == "on") {
        input = psiServer_.session()->getData("input-file");
        inputFromFile_ = true;
    }
    else {
        inputFromFile_ = false;
    }

    if (input.empty()) {
        input = initialText_;
    }

    return input;
}

std::string PipeSite::getPipe_() {
    std::string pipe = psiServer_.session()->getData("pipe-text");

    if (pipe.empty()) {
        pipe = initialPipe_;
    }

    return pipe;
}

char * PipeSite::actionPipe() {
    std::string input = getInput_();
    std::string pipe = getPipe_();

    std::string output = runPipe_(pipe, input);
    psiServer_.session()->setData("output-text", output);

    if (initialOutput_.empty())
        initialOutput_ = output;

    return stringToChar(std::string("/psitoolkit.html"));
}

char * PipeSite::hiddenOptions() {
    std::string fileOnOff = psiServer_.session()->getData("radio-file");
    std::string outputFile = psiServer_.session()->getData("output-file");
    std::string outputType = psiServer_.session()->getData("output-type");
    std::string errorOccured = psiServer_.session()->getData("error-occured");

    std::string opts =
        std::string("psisOptions = {") +
        std::string("'isInputFile' : '") + fileOnOff + std::string("', ") +
        std::string("'fileToDownload' : '") + outputFile + std::string("', ") +
        std::string("'initialOutputType' : '") + outputType + std::string("', " ) +
        std::string("'errorOccured' : '") + errorOccured + std::string("' ") +
        std::string("};");

    psiServer_.session()->clearData("radio-file");

    return stringToChar(opts);
}

std::string PipeSite::getOrSetDefaultData_(const char* name, std::string initialValue) {
    if (!psiServer_.session()->isData(name)) {
        psiServer_.session()->setData(name, initialValue);
    }
    return psiServer_.session()->getData(name);
}

std::string PipeSite::runPipe_(std::string pipe,
                               std::string input,
                               bool withSession /* = true */) {
    preparePipelineAndInput_(pipe, input);

    std::istringstream iss(input);
    std::ostringstream oss;

    INFO("Constructing pipe [" << pipe << "]...");
    INFO("Input: " << input);

    if (withSession) {
        clearPreviousFileFromOutput_();
    }

    if (input.size() > MAX_INPUT_SIZE) {
        oss << "There are some problems: " << TOO_LARGE_INPUT_MSG;

        if (withSession) {
            psiServer_.session()->setData("error-occured", TOO_LARGE_INPUT_MSG);
        }

        return oss.str();
    }

    try {
        PipeRunner p(pipe);

        INFO("... running");
        p.run(iss, oss);
        INFO("... OK");

        if (withSession) {
            createFileFromOutput_(oss.str());
        }
    }
    catch (std::exception& e) {
        oss << "There are some problems: " << e.what();

        if (withSession) {
            psiServer_.session()->setData("error-occured", e.what());
        }
    }

    return oss.str();
}

void PipeSite::preparePipelineAndInput_(std::string& pipe, std::string& input) {
    boost::replace_all(pipe, " | ", " ! ");

    if (input.empty()) {
        input = initialText_;
        return;
    }

    if (inputFromFile_ == true) {
        tryToAddGuessingReader_(pipe);

        boost::replace_first(pipe, "--encoding-conversion ", " ");
        boost::replace_first(pipe, "-c ", " ");
        input = encodingConverter_.convert(input);
    }
}

void PipeSite::tryToAddGuessingReader_(std::string& pipe) {
    size_t found = pipe.find("read");
    if (found != std::string::npos) {
        return;
    }

    std::set<std::string> aliases =
        MainFactoriesKeeper::getInstance().getAllAliases(GUESSING_READER);

    BOOST_FOREACH(std::string alias, aliases) {
        if (boost::starts_with(pipe, alias)) {
            return;
        }
    }

    DEBUG("guessing-reader added to pipeline");
    pipe = GUESSING_READER + " ! " + pipe;
}

void PipeSite::clearPreviousFileFromOutput_() {
    psiServer_.session()->clearData("output-file");
    psiServer_.session()->clearData("error-occured");
}

void PipeSite::createFileFromOutput_(const std::string& output) {
    std::string filename = fileStorage_.storeFile(output);
    psiServer_.session()->setData("output-file", filename);
}

std::string PipeSite::generateOutput_(const std::string& rawOutput) {
    std::ostringstream output;

    std::string type;
    std::string ext;
    fileRecognizer_.recognizeMimeTypeAndFileExtension(rawOutput, type, ext);

    if (type == "image") {
        if (ext == "svg") {
            output << rawOutput;
        }
        else {
            output << "<a href=\"" << psiServer_.session()->getData("output-file")
                << "\" target=\"_blank\" >"
                << "<img src=\"" << psiServer_.session()->getData("output-file")
                << "\" alt=\"image output\" />"
                << "</a>";
        }
    }
    if (type == "application") {
        output << "No output preview.";
    }
    if (type == "text") {
        output << "<pre>" << encodeHTML_(rawOutput) << "</pre>";
    }
    if (type == FileRecognizer::UNKNOWN_TYPE) {
        output << "<pre>" << rawOutput << "</pre>";
        output << "[output format not recognized]";
    }

    return output.str();
}

std::string PipeSite::encodeHTML_(const std::string& data) {
    std::string buffer;
    buffer.reserve(data.size());

    for (size_t pos = 0; pos != data.size(); ++pos) {
        switch (data[pos]) {
            case '&':  buffer.append("&amp;");      break;
            case '\"': buffer.append("&quot;");     break;
            case '\'': buffer.append("&apos;");     break;
            case '<':  buffer.append("&lt;");       break;
            case '>':  buffer.append("&gt;");       break;
            default:   buffer.append(1, data[pos]); break;
        }
    }

    return buffer;
}
