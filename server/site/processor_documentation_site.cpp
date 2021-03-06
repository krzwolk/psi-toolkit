#include <string>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>

#include "processor_documentation_site.hpp"
#include "pipe_runner.hpp"
#include "session_manager.hpp"
#include "html_help_formatter.hpp"

const std::string ProcessorDocumentationSite::PROCESSOR_NAME_PARAM = "name";
const std::string ProcessorDocumentationSite::PROCESSOR_NOT_FOUND = "processor not found";

ProcessorDocumentationSite::ProcessorDocumentationSite(PsiServer& server)
    : HelpTemplateSite(server),
    name_(PROCESSOR_NOT_FOUND),
    documentation_("")
{
    htmlHelpFormatter_.setUseJavaScript(false);

    psiServer_.registerIncludeCode("processor_documentation_site_processor_name",
        boost::bind(&ProcessorDocumentationSite::processorName, this));
    psiServer_.registerIncludeCode("processor_documentation_site_processor_full_name",
        boost::bind(&ProcessorDocumentationSite::processorFullName, this));
    psiServer_.registerIncludeCode("processor_documentation_site_processor_documentation",
        boost::bind(&ProcessorDocumentationSite::processorDocumentation, this));
    psiServer_.registerIncludeCode("processor_documentation_site_processor_documentation_menu",
        boost::bind(&ProcessorDocumentationSite::processorDocumentationMenu, this));

    psiServer_.registerActionCode("help/processor",
        boost::bind(&ProcessorDocumentationSite::actionProcessorDocumentation, this));
}

char * ProcessorDocumentationSite::processorName() {
    return stringToChar(name_);
}

char * ProcessorDocumentationSite::processorFullName() {
    std::string fullName = "unknown processor";

    if (name_ != PROCESSOR_NOT_FOUND) {
        fullName = htmlHelpFormatter_.getProcessorFullName(name_);
    }
    if (fullName.empty()) {
        fullName = name_;
    }

    fullName = std::string("<span class=\"fullName\">") + fullName + std::string("</span>");
    return stringToChar(fullName);
}

char * ProcessorDocumentationSite::processorDocumentation() {
    if (documentation_.str().empty()) {
        documentation_ << pageNotFoundMessage();
    }
    return stringToChar(documentation_.str());
}

char * ProcessorDocumentationSite::actionProcessorDocumentation() {
    documentation_.str(std::string());

    if (psiServer_.session()->isData(PROCESSOR_NAME_PARAM)) {
        name_ = psiServer_.session()->getData(PROCESSOR_NAME_PARAM);

        if (not htmlHelpFormatter_.formatProcessorHelpsByName(name_, documentation_)) {
            documentation_ << "Processor <code>" << name_ << "</code> not found." << std::endl;
            name_ = PROCESSOR_NOT_FOUND;
        }
    }
    else {
        name_ = PROCESSOR_NOT_FOUND;
        documentation_ << "Processor not specified.";
    }

    psiServer_.session()->clearData(PROCESSOR_NAME_PARAM);
    return stringToChar(std::string("/help/processor-documentation.html"));
}

char * ProcessorDocumentationSite::processorDocumentationMenu() {
    std::ostringstream output;

    output << "<ul class=\"processor-list\">" << std::endl;
    BOOST_FOREACH(std::string processorName, htmlHelpFormatter_.getProcessorNames()) {
        output << "<li><a href=\"/help/processor.psis?name=" << processorName << "\">"
            << processorName << "</a></li>" << std::endl;
    }
    output << "</ul>" << std::endl;

    return stringToChar(output.str());
}
