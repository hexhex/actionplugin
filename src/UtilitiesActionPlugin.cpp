/**
 * @file UtilitiesActionPlugin.cpp
 * @author Stefano Germano
 *
 * @brief A Plugin that contains some useful Action and External Atoms
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */
#include "../examples/Utilities/include/UtilitiesActionPlugin.h"

DLVHEX_NAMESPACE_BEGIN

UtilitiesActionPlugin::Environment::Environment() {
	iteration = 1;
}

UtilitiesActionPlugin::Environment::~Environment() {

}
unsigned long UtilitiesActionPlugin::Environment::getIteration() const {
	return iteration;
}

void UtilitiesActionPlugin::Environment::increaseIteration() {
	iteration++;
}

UtilitiesActionPlugin::IterationExternalAtom::IterationExternalAtom() :
		PluginActionAtom("iteration") {
	setOutputArity(1);
}

void UtilitiesActionPlugin::IterationExternalAtom::retrieve(
		const Environment& environment, const Query& query, Answer& answer) {

	Registry &registry = *getRegistry();

	if (query.input.size() != 0)
		throw PluginError("Wrong input argument type (arity in retrieve)");

	std::stringstream concatstream;
	concatstream << environment.getIteration();

	Tuple out;
	Term term(ID::MAINKIND_TERM | ID::SUBKIND_TERM_CONSTANT,
			std::string(concatstream.str()));
	out.push_back(registry.storeTerm(term));
	answer.get().push_back(out);

}

UtilitiesActionPlugin::IterationActionAtom::IterationActionAtom() :
		PluginAction("iterationActionAtom") {

}

void UtilitiesActionPlugin::IterationActionAtom::execute(
		Environment& environment, RegistryPtr pregistry, const Tuple& parms,
		const InterpretationConstPtr interpretationPtr) {

	Registry& registry = *pregistry;

//	if (registry.getTermStringByID(parms[0]) == "increaseIteration")
//		environment.increaseIteration();
		if (registry.getTermStringByID(parms[0]) == "print")
			std::cout << environment.getIteration() << std::endl;

}

std::vector<PluginAtomPtr> UtilitiesActionPlugin::createAtoms(
		ProgramCtx& ctx) const {
	std::vector < PluginAtomPtr > ret;
	ret.push_back(
			PluginAtomPtr(new IterationExternalAtom,
					PluginPtrDeleter<PluginAtom>()));
	return ret;
}

std::vector<PluginActionBasePtr> UtilitiesActionPlugin::createActions(
		ProgramCtx& ctx) const {
	std::vector<PluginActionBasePtr> ret;
	ret.push_back(
			PluginActionBasePtr(new IterationActionAtom,
					PluginPtrDeleter<PluginActionBase>()));
	return ret;
}

//
// now instantiate the plugin
//
UtilitiesActionPlugin theUtilitiesActionPlugin;

DLVHEX_NAMESPACE_END

//
// let it be loaded by dlvhex!
//
IMPLEMENT_PLUGINABIVERSIONFUNCTION

// return plain C type s.t. all compilers and linkers will like this code
extern "C" void * PLUGINIMPORTFUNCTION() {
return reinterpret_cast<void*>(& DLVHEX_NAMESPACE theUtilitiesActionPlugin);
}
