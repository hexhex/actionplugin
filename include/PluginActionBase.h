/**
 * @file PluginActionBase.h
 * @author Stefano Germano
 *
 * @brief ...
 */

#ifndef PLUGIN_ACTION_BASE_H_
#define PLUGIN_ACTION_BASE_H_

DLVHEX_NAMESPACE_BEGIN

class Environment;

class PluginActionBase {
public:
	PluginActionBase(const std::string& predicate) :
			predicate(predicate) {
	}
	virtual ~PluginActionBase() {
	}
	const std::string& getPredicate() const {
		return predicate;
	}

#warning a test, must be fixed
	void execute(const RegistryPtr registryPtr, const Tuple& tuple);

protected:
	virtual void execute(Environment&, const RegistryPtr registryPtr,
			const Tuple&, const InterpretationConstPtr);
	std::string predicate;
};
typedef boost::shared_ptr<PluginActionBase> PluginActionBasePtr;

DLVHEX_NAMESPACE_END

#endif /* PLUGIN_ACTION_BASE_H_ */
