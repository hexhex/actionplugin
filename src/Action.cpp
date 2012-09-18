/**
 * @file Action.cpp
 * @author Stefano Germano
 *
 * @brief A simple class to store an Action
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H
#include "acthex/Action.h"

DLVHEX_NAMESPACE_BEGIN

Action::Action() :
		predicate(""), aux_id(ID_FAIL) {
}

Action::Action(std::string predicate, ID aux_id) :
		predicate(predicate), aux_id(aux_id) {
}

Action::~Action() {
}

ID Action::getAuxId() const {
	return aux_id;
}

std::string Action::getPredicate() const {
	return predicate;
}

DLVHEX_NAMESPACE_END
