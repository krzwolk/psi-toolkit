#ifndef FSA_NDFSA_IMPL_HDR
#define FSA_NDFSA_IMPL_HDR

namespace fsa {

template <typename ArcT>
NDFSA<ArcT>::NDFSA() {}

template <typename ArcT>
NDFSA<ArcT>::NDFSA(typename ArcT::symbol_type s) {
    state_type q0 = addState(true);
    state_type q1 = addState();

    addArc(q0, ArcT(s, q1));
    setEndState(q1);
}

template <typename ArcT>
template <typename InputIterator>
NDFSA<ArcT>::NDFSA(InputIterator it, InputIterator end) {
    state_type q0 = addState(true);
    state_type p, q = q0;

    while (it != end) {
        p = q;
        q = addState();

        addArc(p, ArcT(*it, q));
        it++;
    }
    setEndState(q);
}

template <typename ArcT>
NDFSA<ArcT>::NDFSA(std::string input) {
    state_type q0 = addState(true);
    state_type p, q = q0;

    for (std::string::iterator it = input.begin(); it != input.end(); it++) {
        p = q;
        q = addState();

        addArc(p, ArcT(*it, q));
    }
    setEndState(q);
}

template <typename ArcT>
NDFSA<ArcT>::~NDFSA() {
    BOOST_FOREACH( Arcs* arcs, m_states )
        delete arcs;
}

template <typename ArcT>
inline std::set<typename ArcT::state_type> NDFSA<ArcT>
    ::deltaPrime(typename ArcT::state_type p, typename ArcT::symbol_type a) const {

    if (m_states.size() < (size_t)p)
        return std::set<state_type>();

    std::set<state_type> destinations;
    arc_iterator_type arc = find(p, a);
    while (arc != m_states[p]->end() && arc->getSymbol() == a) {
        destinations.insert(arc->getDest());
        arc++;
    }
    return destinations;
}

template <typename ArcT>
inline std::set<typename ArcT::state_type> NDFSA<ArcT>
    ::delta(std::set<typename ArcT::state_type> pSet, typename ArcT::symbol_type a) const {

    std::set<state_type> destinations;
    BOOST_FOREACH(state_type p, pSet) {
        std::set<state_type> destPrime = deltaPrime(p, a);
        destinations.insert(destPrime.begin(), destPrime.end());
    }
    return destinations;
}

template <typename ArcT>
inline typename NDFSA<ArcT>::arc_iterator_type
NDFSA<ArcT>::find(typename ArcT::state_type p, typename ArcT::symbol_type a) const {
    ArcRange<arc_iterator_type> r = getArcs(p);

    ArcT test(a, 0);
    arc_iterator_type result = std::lower_bound(r.first, r.second, test, ArcSorter());

    //std::cerr << "Arc find: " << (int)a << " " << std::distance(r.first, result) << std::endl;
    //std::cerr << "Check: " << (result != r.second && a == result->getSymbol()) << std::endl;
    return (result != r.second && a == result->getSymbol()) ? result : r.second;
}

template <typename ArcT>
template <typename InputIterator>
bool NDFSA<ArcT>::in(InputIterator it, InputIterator end) {
    std::set<state_type> current_states = getStartStates();

    while (it != end) {
        std::set<state_type> next_states = delta(current_states, *it);
        if (next_states.size())
            current_states = next_states;
        else
            return false;
        it++;
    }
    BOOST_FOREACH(state_type p, current_states)
        if (isEndState(p))
            return true;
    return false;
}

template <typename ArcT>
typename ArcT::state_type NDFSA<ArcT>::addState(bool isStartState) {
    m_states.push_back(new Arcs());
    if (isStartState)
        m_startStates.insert(state_type(m_states.size()-1));

    return m_states.size()-1;
}

template <typename ArcT>
void NDFSA<ArcT>::deleteLastState() {
    State state = m_states.size() - 1;
    delete m_states.back();
    m_states.pop_back();
    m_startStates.erase(state);
    m_endStates.erase(state);
}

template <typename ArcT>
const std::set<typename ArcT::state_type>& NDFSA<ArcT>::getStartStates() const {
    return m_startStates;
}

template <typename ArcT>
bool NDFSA<ArcT>::isStartState(typename ArcT::state_type state) const {
    return m_startStates.count(state);
}

template <typename ArcT>
void NDFSA<ArcT>::setStartState(typename ArcT::state_type state) {
    m_startStates.insert(state);
}

template <typename ArcT>
void NDFSA<ArcT>::unsetStartState(typename ArcT::state_type state) {
    m_startStates.erase(state);
}

template <typename ArcT>
const std::set<typename ArcT::state_type>& NDFSA<ArcT>::getEndStates() const {
    return m_endStates;
}

template <typename ArcT>
bool NDFSA<ArcT>::isEndState(typename ArcT::state_type state) const {
    return m_endStates.count(state);
}

template <typename ArcT>
void NDFSA<ArcT>::setEndState(typename ArcT::state_type state) {
    m_endStates.insert(state);
}

template <typename ArcT>
void NDFSA<ArcT>::unsetEndState(typename ArcT::state_type state) {
    m_endStates.erase(state);
}

template <typename ArcT>
void NDFSA<ArcT>::addArc(typename ArcT::state_type state, ArcT arc) {
    //std::cerr << "Adding: " << state << " " << arc.getSymbol() << " " << arc.getDest() << std::endl;
    m_states[state]->push_back(arc);
    std::sort(m_states[state]->begin(), m_states[state]->end(), ArcSorter());
}

template <typename ArcT>
const ArcRange<typename NDFSA<ArcT>::arc_iterator_type>
NDFSA<ArcT>::getArcs(typename ArcT::state_type p) const {
    return ArcRange<arc_iterator_type>(
            m_states[p]->begin(),
            m_states[p]->end()
        );
}

template <typename ArcT>
size_t NDFSA<ArcT>::size() const {
    return m_states.size();
}

template <typename ArcT>
void NDFSA<ArcT>::swap(NDFSA<ArcT> &fsa) {
    m_states.swap(fsa.m_states);
    m_startStates.swap(fsa.m_startStates);
    m_endStates.swap(fsa.m_endStates);
}

template <typename ArcT>
void NDFSA<ArcT>::print() {
    for (size_t i = 0; i < m_states.size(); i++) {
        ArcRange<arc_iterator_type> r = getArcs(i);
        for (arc_iterator_type it = r.first; it != r.second; it++) {
            std::cout << i << "\t" << it->getDest() << "\t" << (char)it->getSymbol()
                << std::endl;
        }
        //if (isStartState(i)) {
        //    std::cout << "s(" << i << ")" << std::endl;
        //}
        if (isEndState(i)) {
            std::cout << i << std::endl;
        }
    }
}

template <typename ArcT>
std::set<typename ArcT::state_type> NDFSA<ArcT>::epsClosure(typename ArcT::state_type p) const {
    std::set<state_type> closure;
    std::vector<state_type> queue;

    closure.insert(p);
    queue.push_back(p);
    while (!queue.empty()) {
        state_type s = queue.back();
        queue.pop_back();

        ArcRange<arc_iterator_type> arcs = getArcs(s);
        for (arc_iterator_type ait = arcs.first; ait != arcs.second; ait++) {
            if (ait->getSymbol() == EPS && !closure.count(ait->getDest())) {
                closure.insert(ait->getDest());
                queue.push_back(ait->getDest());
            }
        }
    }

    return closure;
}

template <typename ArcT>
NDFSA<ArcT>& NDFSA<ArcT>::operator=(const NDFSA<ArcT> &a) {
    BOOST_FOREACH(Arcs* arcs, a.m_states)
        m_states.push_back(new Arcs(*arcs));

    m_endStates = a.m_endStates;
    m_startStates = a.m_startStates;

    return *this;
}

}

#endif
