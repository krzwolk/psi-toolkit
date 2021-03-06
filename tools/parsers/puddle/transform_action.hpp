#ifndef TRANSFORMACTION_HDR
#define TRANSFORMACTION_HDR

#include "action.hpp"

namespace poleng {

namespace bonsai {

namespace puddle {

class TransformAction : public Action {
    public:
        TransformAction(std::string aGroup, int aElement, std::string aRuleName);
        bool apply(Lattice &lattice, std::string langCode,
                int matchedStartIndex, RuleTokenSizes &ruleTokenSizes,
                std::list<Lattice::EdgeSequence> &rulePartitions);
        bool test(Lattice &lattice, std::string langCode,
                int matchedStartIndex, RuleTokenSizes &ruleTokenSizes,
                std::list<Lattice::EdgeSequence> &rulePartitions);

    private:
        void init(std::string aGroup, int aElement, std::string aRuleName);

        std::string group;
        int element;
        std::string ruleName;
};

typedef boost::shared_ptr<TransformAction> TransformActionPtr;

}

}

}

#endif
