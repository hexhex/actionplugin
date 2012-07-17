/**
 * @file Action.cpp
 * @author Stefano Germano
 *
 * @brief ...
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "Action.h"

DLVHEX_NAMESPACE_BEGIN

Action::Action() :
predicate("") {}
// I don't know if it's better initialize this->aux_id at null

Action::Action(std::string predicate, dlvhex::ID aux_id) :
predicate(predicate), aux_id(aux_id) {
}

Action::~Action() {
}

dlvhex::ID Action::getAuxId() const {
  return aux_id;
}

std::string Action::getPredicate() const {
  return predicate;
}

DLVHEX_NAMESPACE_END
