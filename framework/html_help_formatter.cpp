#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/replace.hpp>

#include "html_help_formatter.hpp"
#include "processors_helper.hpp"
#include "shallow_aliaser.hpp"
#include "string_inflection.hpp"
#include "logging.hpp"

#include "sundown/cpp/stringwrapper.hpp"

std::set<std::string> HtmlHelpFormatter::extensionsForRandomExamples_ =
    boost::assign::list_of("txt")("html");

HtmlHelpFormatter::HtmlHelpFormatter()
    : fileStorage_(NULL),
    useJavaScript_(true) { }

HtmlHelpFormatter::HtmlHelpFormatter(bool useJavaScript)
    : fileStorage_(NULL),
    useJavaScript_(useJavaScript) { }

void HtmlHelpFormatter::setUseJavaScript(bool onOff) {
    useJavaScript_ = onOff;
}

void HtmlHelpFormatter::doFormatOneProcessorHelp(
    std::string processorName,
    std::string description,
    std::string detailedDescription,
    boost::program_options::options_description options,
    std::list<std::string> aliases,
    std::vector<TestBatch> usingExamples,
    std::list<std::string> languagesHandled,
    std::ostream& output) {

    output << "<div class=\"help-item\">"
        << "<h2 id=\"" << processorName << "\">" << processorName << "</h2>" << std::endl;

    formatDescription_(description, detailedDescription, processorName, output);

    if (!aliases.empty()) {
        formatAliases_(aliases, output);
    }

    if (!languagesHandled.empty()) {
        formatLanguagesHandled_(languagesHandled, output);
    }

    if (!usingExamples.empty()) {
        formatUsingExamples_(usingExamples, output);
    }

    if (!areOptionsEmpty(options)) {
        formatAllowedOptions_(options, output);
    }
    output << "</div>" << std::endl;
}

void HtmlHelpFormatter::formatDescription_(const std::string& description,
                                           const std::string& details,
                                           const std::string& processorName,
                                           std::ostream& output) {
    if (!description.empty()) {
        output << "<div class=\"help-desc\">" << markdownString2String(description);

        if (!details.empty()) {
            output << "<div class=\"help-details\""
                << (useJavaScript_ ? " style=\"display:none;\"" : "") << ">"
                << markdownString2String(details)
                << "</div>" << std::endl
                << (useJavaScript_
                    ? "<span class=\"toggler help-toggler\"> [show more] </span>" : "")
                << std::endl;
        }

        if (useJavaScript_) {
            output << "<a href=\"/help/processor.psis?name=" << processorName << "\" "
                << "class=\"toggler\" target=\"_blank\"> [open in single page] </a>" << std::endl;
        }

        output << "</div>" << std::endl;
    }
}

void HtmlHelpFormatter::formatLanguagesHandled_(std::list<std::string> langCodes,
                                                std::ostream& output) {
    output << "<div class=\"help-langs\">" << std::endl
        << "<h3>" << LANGUAGES_HEADER << "</h3>" << std::endl
        << "<span>" << boost::algorithm::join(langCodes, ", </span><span>") << "</span>"
        << "</div>" << std::endl;
}

void HtmlHelpFormatter::formatAliases_(std::list<std::string> aliases, std::ostream& output) {
    output << "<div class=\"help-alias\">"
        << "<h3>" << ALIASES_HEADER << "</h3>"
        << boost::algorithm::join(aliases, ", ")
        << "</div>" << std::endl;
}

void HtmlHelpFormatter::formatUsingExamples_(std::vector<TestBatch> batches, std::ostream& output) {
    output << "<div class=\"help-examples\">"
        << "<h3>" << EXAMPLES_HEADER
            //<< " <span class=\"toggler example-toggler\">[show]</span>"
        << "</h3>"
        << std::endl;

    for (unsigned int i = 0; i < batches.size(); i++) {
        output << "<div class=\"help-example\">";

        output << "<code class=\"example-pipe\">"
            << escapeHTML_(batches[i].getPipeline()) << "</code>" << std::endl;

        output << "<div class=\"example-desc\">"
            << markdownString2String(batches[i].getDescription()) << "</div>" << std::endl;

        std::vector<TestRun> inOuts = batches[i].getTestRuns();

        for (unsigned int j = 0; j < inOuts.size(); j++) {
            output << "<div class=\"in-out\">" << std::endl;

            formatExampleInputOutput_(inOuts[j].getInputFilePath(), output, "in");
            formatExampleInputOutput_(inOuts[j].getExpectedOutputFilePath(), output, "out");

            output << "</div>" << std::endl;
        }

        output << "</div>";
    }

    output << "</div>" << std::endl;
}

void HtmlHelpFormatter::formatExampleInputOutput_(
    const boost::filesystem::path& filePath,
    std::ostream& output,
    const std::string& divClass) {

    output << "<div class=\"" << divClass << "\">" << divClass << ":</div>" << std::endl;
    std::string fileContent = getFileContent(filePath);

    std::string type;
    std::string ext;
    fileRecognizer_.recognizeMimeTypeAndFileExtension(fileContent, type, ext);

//    if (type == "image" && ext == "svg") {
//        output << fileContent << std::endl;
//        return;
//    }
    if ((type == "text" && ext == "txt" ) || type == FileRecognizer::UNKNOWN_TYPE) {
        output << "<pre><code>" << escapeHTML_(fileContent) << "</code></pre>" << std::endl;
        return;
    }
    if (type == "application" && ext == "zip") {
        ext = fileRecognizer_.recognizeCompressedFileFormat(fileContent);
    }

    if (fileStorage_ != NULL) {
        std::string path = (*fileStorage_).storeFileByMD5(fileContent, ext);

        output << "<a href=\"" << path << "\" target=\"_blank\" >";

        if (type == "image" && ext == "svg") {
            output << fileContent << std::endl;
        }
        else if (type == "image" && ext != "djvu") {
            output << "<img src=\"" << path << "\" alt=\"image output\" />";
        } else {
            output << path;
        }
        output << "</a>" << std::endl;
    }
    else {
        output << "<pre><code>" << escapeHTML_(fileContent) << "</code></pre>" << std::endl;
    }
}

void HtmlHelpFormatter::doFormatDataFile(std::string text, std::ostream& output) {
    output << markdownString2String(text) << std::endl;
}

void HtmlHelpFormatter::formatPipelineExamplesInJSON(std::ostream& output) {
    output << "var pipelineExamples = [" << std::endl;

    BOOST_FOREACH(std::string processorName, getProcessorNames()) {
        BOOST_FOREACH(TestBatch testBatch, getProcessorUsingExamples(processorName)) {
            formatPipelineExampleInJSON_(testBatch, output);
        };
    }

    output << "];" << std::endl;
}

void HtmlHelpFormatter::formatPipelineExampleInJSON_(TestBatch batch, std::ostream& output) {
    std::string text = getFileContent(batch.getTestRuns()[0].getInputFilePath());
    std::string extension = fileRecognizer_.recognizeFileExtension(text);

    if (extensionsForRandomExamples_.find(extension) == extensionsForRandomExamples_.end()) {
        INFO("random example \"" << batch.getDescription()
            << "\" is skipping because of input file extension");
        return;
    }

    output << "  {" << std::endl;

    std::string description = batch.getDescription();
    output << "    \"description\" : \"" << escapeJSON_(description) << "\"," << std::endl;

    std::string pipe = batch.getPipeline();
    boost::algorithm::trim(pipe);
    output << "    \"pipe\" : \"" << escapeJSON_(pipe) << "\"," << std::endl;

    boost::algorithm::trim(text);
    output << "    \"text\" : \"" << escapeJSON_(text) << "\"" << std::endl;

    output << "  }, " << std::endl;
}

void HtmlHelpFormatter::formatHelpsWithTypes(std::ostream& output) {
    std::vector<std::string> types = ProcessorsHelper::getAllProcessorTypes_();

    BOOST_FOREACH(std::string type, types) {
        output << "<h1 class=\"type-header\">"
            << pluralizeRegularWord(type)
            << "</h1>" << std::endl
            << "<div class=\"type-container\">" << std::endl;

        std::vector<std::string> subtypes = ProcessorsHelper::getAllSubTypesForProcessorType_(type);

        BOOST_FOREACH(std::string subtype, subtypes) {
            if (!subtype.empty()) {
                output << "<h1 class=\"subtype-header\">"
                    << pluralizeRegularWord(subtype)
                    << "</h1>" << std::endl
                    << "<div class=\"subtype-container\">" << std::endl;
            }

            std::vector<std::string> processors =
                ProcessorsHelper::getAllProcessorsNamesForTypeAndSubType_(type, subtype);

            BOOST_FOREACH(std::string processor, processors) {
                formatOneProcessorHelp(processor, output);
            }

            if (!subtype.empty()) {
                output << "</div>" << std::endl;
            }
        }

        output << "</div>" << std::endl;
    }
}

void HtmlHelpFormatter::formatAllowedOptions_(boost::program_options::options_description options,
    std::ostream& output) {

    output << "<div class=\"help-opts\">"
        << "<h3>" << OPTIONS_HEADER << "</h3>" << std::endl
        << "<pre>" << options << "</pre>" << std::endl
        << "</div>" << std::endl;
}

void HtmlHelpFormatter::doFormatOneAlias(
    std::string aliasName,
    std::list<std::string> processorNames,
    std::ostream& output) {

    if (processorNames.empty())
        return;

    output << "<div class=\"alias-item\">";
    if (processorNames.size() > 1) {
        output << "<a href=\"/help/processor.psis?name=" << aliasName << "\">";
    }
    output << aliasName;
    if (processorNames.size() > 1) {
        output << "</a>";
    }
    output << " &rarr; ";

    unsigned int i = 0;

    BOOST_FOREACH(std::string processorName, processorNames) {
        output << "<a href=\"/help/processor.psis?name="
            << getProcessorNameWithoutOptions(processorName) << "\">"
            << processorName << "</a>";

        if (++i != processorNames.size())
            output << ", ";
    }

    output << "</div>" << std::endl;
}

HtmlHelpFormatter::~HtmlHelpFormatter() { }

std::string HtmlHelpFormatter::escapeHTML_(const std::string& text) {
    std::string buffer;
    buffer.reserve(text.size());

    for (size_t pos = 0; pos != text.size(); ++pos) {
        switch (text[pos]) {
            case '&':  buffer.append("&amp;");      break;
            case '\"': buffer.append("&quot;");     break;
            case '\'': buffer.append("&apos;");     break;
            case '<':  buffer.append("&lt;");       break;
            case '>':  buffer.append("&gt;");       break;
            default:   buffer.append(1, text[pos]); break;
        }
    }

    return buffer;
}

std::string HtmlHelpFormatter::escapeJSON_(std::string& text) {
    boost::replace_all(text, "\\", "\\\\");
    boost::replace_all(text, "\"", "\\\"");
    boost::replace_all(text, "\n", "\\n");

    return text;
}

void HtmlHelpFormatter::setFileStorage(FileStorage* fileStorage) {
    fileStorage_ = fileStorage;
}

void HtmlHelpFormatter::unsetFileStorage() {
    fileStorage_ = NULL;
}
