/**
 * @file PluginActionBase.h
 * @author Stefano Germano
 *
 * @brief Base class to implement a PluginAction
 */

#ifndef PLUGIN_ACTION_BASE_H_
#define PLUGIN_ACTION_BASE_H_

#include "dlvhex2/PluginInterface.h"

DLVHEX_NAMESPACE_BEGIN

// Forward declaration
class Environment;

class PluginActionBase {
public:
	PluginActionBase(const std::string& predicate) :
			predicate(predicate) {
	}

	virtual ~PluginActionBase();

	const std::string& getPredicate() const {
		return predicate;
	}

	// The function that will be called by the FinalCallback to execute the Actions
	virtual void execute(ProgramCtx& ctx,
			const InterpretationConstPtr interpretationConstPtr,
			const Tuple& tuple) {}
protected:
	// The function that must be overridden by the creator of the Action to execute the own code
	virtual void execute(Environment&, RegistryPtr registryPtr, const Tuple&,
			const InterpretationConstPtr interpretationConstPtr) = 0;

	// a string that will be used to identify it
	std::string predicate;
};
typedef boost::shared_ptr<PluginActionBase> PluginActionBasePtr;

DLVHEX_NAMESPACE_END

#endif /* PLUGIN_ACTION_BASE_H_ */
