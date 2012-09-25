/**
 * @file Action.h
 * @author Stefano Germano
 *
 * @brief A simple class to store an Action
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
	/**
	 * @brief  The string of the ID of this Action
	 */
	std::string predicate;
	/**
	 * @brief  An auxiliary id for this Action taken from the Registry
	 */
	dlvhex::ID aux_id;
};
typedef boost::shared_ptr<Action> ActionPtr;

DLVHEX_NAMESPACE_END

#endif /* ACTION_H_ */
