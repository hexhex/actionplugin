/**
 * @file PluginActionBase.h
 * @author Stefano Germano
 *
 * @brief ...
 */

#ifndef PLUGIN_ACTION_BASE_H_
#define PLUGIN_ACTION_BASE_H_

#include "dlvhex2/PluginInterface.h"

DLVHEX_NAMESPACE_BEGIN

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

	virtual void execute(ProgramCtx& ctx,
			const InterpretationConstPtr interpretationConstPtr,
			const Tuple& tuple) {
	}
	;

protected:
	virtual void execute(Environment&, RegistryPtr registryPtr, const Tuple&,
			const InterpretationConstPtr interpretationConstPtr) {
	}
	;
	std::string predicate;
};
typedef boost::shared_ptr<PluginActionBase> PluginActionBasePtr;

DLVHEX_NAMESPACE_END

#endif /* PLUGIN_ACTION_BASE_H_ */