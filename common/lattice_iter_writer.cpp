#include "lattice_iter_writer.hpp"

LatticeIterWriter::LatticeIterWriter(
    Lattice& lattice,
    LatticeWriterOutputIterator& outputIterator,
    bool linear,
    std::string basicTag,
    std::vector<std::string> handledTags
) :
    lattice_(lattice),
    outputIterator_(outputIterator),
    linear_(linear),
    basicTag_(basicTag),
    handledTags_(handledTags)
{ }

LatticeIterWriter::~LatticeIterWriter() {
}

void LatticeIterWriter::run() {
    doRun();
}

std::string LatticeIterWriter::doInfo() {
    return "Lattice iter writer";
}

void LatticeIterWriter::doRun() {

    Lattice::VertexIterator vi(lattice_);
    Lattice::VertexDescriptor vd = lattice_.getFirstVertex();
    Lattice::EdgeDescriptor edge;
    Lattice::EdgeDescriptor basicTagEdge;

    std::map<std::string, Lattice::VertexDescriptor> targets;

    while (linear_ ? true : vi.hasNext()) {
        if (!linear_) {
            vd = vi.next();
        }
        Lattice::InOutEdgesIterator oei
            = lattice_.outEdges(vd, lattice_.getLayerTagManager().anyTag());
        bool basicTagFound = false;
        while(oei.hasNext()) {
            edge = oei.next();
            std::list<std::string> tags
                = lattice_.getLayerTagManager().getTagNames(lattice_.getEdgeLayerTags(edge));
            for (
                std::list<std::string>::iterator ti = tags.begin();
                ti != tags.end();
                ++ti
            ) {
                if (basicTag_ == *ti) {
                    basicTagFound = true;
                    basicTagEdge = edge;
                }
                if (isHandledTag_(*ti) && targets[*ti] == vd) {
                    targets[*ti] = lattice_.getEdgeTarget(edge);
                    outputIterator_.closeGroup(*ti);
                    outputIterator_.openGroup(*ti);
                }
            }
        }
        if (basicTagFound) {
            if (lattice_.getAnnotationText(basicTagEdge) == "") {
                outputIterator_.putElement(
                    lattice_.getAnnotationCategory(basicTagEdge)
                );
            } else {
                outputIterator_.putElement(
                    lattice_.getAnnotationText(basicTagEdge)
                );
            }
        }
        outputIterator_.closeAlternative();
        outputIterator_.openAlternative();
        if (linear_) {
            try {
                vd = lattice_.getEdgeTarget(
                    lattice_.firstOutEdge(vd, lattice_.getLayerTagManager().anyTag())
                );
            } catch (NoEdgeException) {
                break;
            }
        }
    }

    for (
        std::map<std::string, Lattice::VertexDescriptor>::iterator mi = targets.begin();
        mi != targets.end();
        ++mi
    ) {
        outputIterator_.closeGroup((*mi).first);
    }

    outputIterator_.flush();

}

bool LatticeIterWriter::isHandledTag_(std::string tagName) {
    return std::find(handledTags_.begin(), handledTags_.end(), tagName) != handledTags_.end();
}