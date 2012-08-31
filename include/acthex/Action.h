/**
 * @file Action.h
 * @author Stefano Germano
 *
 * @brief ...
 */

#ifndef ACTION_H_
#define ACTION_H_

#include "dlvhex2/PlatformDefinitions.h"
#include "dlvhex2/ID.h"

DLVHEX_NAMESPACE_BEGIN

class Action {
public:
	Action();
	Action(std::string, dlvhex::ID);
	virtual ~Action();

	dlvhex::ID getAuxId() const;
	std::string getPredicate() const;

protected:
	std::string predicate;
	dlvhex::ID aux_id;
};
typedef boost::shared_ptr<Action> ActionPtr;

DLVHEX_NAMESPACE_END

#endif /* ACTION_H_ */
