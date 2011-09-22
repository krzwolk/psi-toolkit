#ifndef PIPE_RUNNER_HDR
#define PIPE_RUNNER_HDR

#include <boost/shared_ptr.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "pipeline_specification.hpp"

#include "processor_factory.hpp"
#include "lattice_reader_factory.hpp"
#include "lattice_writer_factory.hpp"
#include "annotator_factory.hpp"

class PipeRunner {

public:
    explicit PipeRunner(const std::string& pipeline);
    PipeRunner(int argc, char* argv[]);
    PipeRunner(std::vector<std::string> args);

    int run(std::istream&, std::ostream&);

private:
    PipelineSpecification pipelineSpecification_;

    void parseIntoGraph_(std::vector<std::string> args, bool isTheFirstArgProgramName);
    void parseIntoPipelineSpecification_(std::vector<std::string> args, bool isTheFirstArgProgramName);

    ProcessorFactory& getFactory_(const PipelineElementSpecification& elementSpec);
    LatticeReaderFactory& getReaderFactory_(const PipelineElementSpecification& elementSpec);
    LatticeWriterFactory& getWriterFactory_(const PipelineElementSpecification& elementSpec);
    AnnotatorFactory& getAnnotatorFactory_(const PipelineElementSpecification& elementSpec);

    bool isLastElement_(
        std::list<PipelineElementSpecification>::iterator it,
        PipelineSpecification& pipelineSpecification);

    boost::program_options::variables_map parseOptions_(
        const boost::program_options::options_description& optionsDescription,
        const PipelineElementSpecification& pipelineElement);

    std::vector<std::string> splitPipeline_(const std::string& pipeline);

    static const std::string PIPELINE_SEPARATOR;


    struct PipelineNode {
        ProcessorFactory* factory_;
        boost::program_options::variables_map options_;
        boost::shared_ptr<Processor> processor_;

        PipelineNode(ProcessorFactory& factory, boost::program_options::variables_map options)
            : factory_(&factory), options_(options) {
        }

        void createProcessor() {
            if (!processor_)
                processor_.reset(factory_->createProcessor(options_));
        }

        boost::shared_ptr<Processor> getProcessor() {
            return processor_;
        }
    };

    PipelineNode pipelineElement2Node_(const PipelineElementSpecification& element);

    typedef boost::adjacency_list<
        boost::vecS,
        boost::listS,
        boost::bidirectionalS,
        PipelineNode,
        std::string
    > PipelineGraph;

    PipelineGraph pipelineGraph_;

    PipelineGraph::vertex_descriptor firstNode;
    PipelineGraph::vertex_descriptor lastNode;

    void pipelineSpecification2Graph_();
    void completeGraph_();
    void runPipelineNode_(
        PipelineGraph::vertex_descriptor current,
        Lattice& lattice,
        std::istream& in, std::ostream& out);
    bool goToNextNode_(PipelineGraph::vertex_descriptor& current);
};

#endif
