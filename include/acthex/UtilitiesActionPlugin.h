/**
 * @file UtilitiesActionPlugin.h
 * @author Stefano Germano
 *
 * @brief A Plugin that contains some useful Action and External Atoms
 */

#ifndef BOOL_MATRIX_ACTION_PLUGIN_H_
#define BOOL_MATRIX_ACTION_PLUGIN_H_

#include "acthex/ActionPluginInterface.h"

DLVHEX_NAMESPACE_BEGIN

class UtilitiesActionPlugin: public ActionPluginInterface {

public:

	UtilitiesActionPlugin() :
			ActionPluginInterface() {
		setNameVersion("libactionutilities", 2, 0, 0);
	}

	class Environment: public PluginEnvironment {
	public:
		Environment();
		virtual ~Environment();
		unsigned long getIteration() const;
		void increaseIteration();
	private:
		unsigned long iteration;
	};

	class IterationExternalAtom: public PluginActionAtom<UtilitiesActionPlugin> {
	public:
		IterationExternalAtom();
	private:
		virtual void retrieve(const Environment& environment,
				const Query& query, Answer& answer);
	};

	class IterationActionAtom: public PluginAction<UtilitiesActionPlugin> {
	public:
		IterationActionAtom();
	protected:
		void execute(Environment&, RegistryPtr, const Tuple&,
				const InterpretationConstPtr);
	};

	virtual std::vector<PluginAtomPtr> createAtoms(ProgramCtx& ctx) const;

	virtual std::vector<PluginActionBasePtr> createActions(
			ProgramCtx& ctx) const;

protected:

	ActionPluginInterfacePtr create(ProgramCtx& ctx) {
		DBGLOG(PLUGIN, "create in IterationActionAtom");
WARNING("a trick, maybe we should find a way remove it")
		ctx.getPluginEnvironment<UtilitiesActionPlugin>();
		DBGLOG(PLUGIN, "getPluginEnvironment done");
		return boost::shared_ptr < UtilitiesActionPlugin
				> (new UtilitiesActionPlugin());
	}

};

DLVHEX_NAMESPACE_END

#endif /* BOOL_MATRIX_ACTION_PLUGIN_H_ */
