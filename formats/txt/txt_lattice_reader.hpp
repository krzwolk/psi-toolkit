#ifndef TXT_LATTICE_READER_HDR
#define TXT_LATTICE_READER_HDR


#include "stream_lattice_reader.hpp"
#include "lattice_reader_factory.hpp"

class TxtLatticeReader : public StreamLatticeReader {

public:
    TxtLatticeReader(bool process_whole_text=false):
        process_whole_text_(process_whole_text) {
    }

    virtual std::string getFormatName();

    class Factory : public LatticeReaderFactory<std::istream> {
    public:
        virtual ~Factory();

    private:
        virtual LatticeReader<std::istream>* doCreateLatticeReader(
            const boost::program_options::variables_map& options);

        virtual boost::program_options::options_description doOptionsHandled();

        virtual std::string doGetName() const;
        virtual boost::filesystem::path doGetFile() const;
    };

private:
    virtual std::string doInfo();

    class Worker : public ReaderWorker<std::istream> {
    public:
        Worker(TxtLatticeReader& processor,
               std::istream& inputStream,
               Lattice& lattice);

        virtual void doRun();

    private:
        TxtLatticeReader& processor_;
        LayerTagCollection textTags_;
    };

    virtual ReaderWorker<std::istream>* doCreateReaderWorker(
        std::istream& inputStream, Lattice& lattice) {

        return new Worker(*this, inputStream, lattice);
    }

    bool process_whole_text_;
};

#endif
