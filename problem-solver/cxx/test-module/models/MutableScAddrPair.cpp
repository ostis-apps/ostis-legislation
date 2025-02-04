#include "sc-memory/sc_addr.hpp"
#include "MutableScAddrPair.hpp"

ScAddr MutableScAddrPair::getFirst() {
    return first;
}

ScAddr MutableScAddrPair::getSecond() {
    return second;
}

bool MutableScAddrPair::operator==(const MutableScAddrPair& other) const {
    return (first == other.first) && (second == other.second);
}

bool MutableScAddrPair::operator<(const MutableScAddrPair& other) const {
    if (first.Hash() < other.first.Hash()) {
        return true;
    }
    if (first.Hash() > other.first.Hash()) {
        return false;
    }
    return second.Hash() < other.second.Hash();
}