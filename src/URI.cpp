//
// Created by Manuel Hahn on 6/14/22.
//

#include "URI.hpp"

URI::URI(const std::string &uri): original(uri) {}

URI::~URI() {}

bool URI::isCGIIdentifier() const {
    return false;
}
