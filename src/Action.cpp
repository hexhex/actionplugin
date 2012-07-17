/**
 * @file Action.cpp
 * @author Stefano Germano
 *
 * @brief ...
 */

#include "Action.h"

DLVHEX_NAMESPACE_BEGIN

Action::Action(std::string predicate, dlvhex::ID aux_id) :
this->predicate(predicate), this->aux_id(aux_id) {}

Action::~Action() {
}

dlvhex::ID getAuxId() const {
  return aux_id;
}

std::string getPredicate() const {
  return predicate;
}

DLVHEX_NAMESPACE_END

