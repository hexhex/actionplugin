/**
 * @file ActionPluginInterface.h
 * @author Stefano Germano
 *
 * @brief Interface to implement a Plugin of ActionPlugin
 */

#ifndef ACTION_PLUGIN_INTERFACE_H_
#define ACTION_PLUGIN_INTERFACE_H_

#include "acthex/ActionPluginCtxData.h"
#include "acthex/PluginActionBase.h"
#include "acthex/BestModelSelector.h"
#include "acthex/ExecutionScheduleBuilder.h"

#include "dlvhex2/PluginInterface.h"
#include "dlvhex2/PlatformDefinitions.h"
#include "dlvhex2/ProgramCtx.h"

DLVHEX_NAMESPACE_BEGIN

class ActionPluginInterface: public PluginInterface {

public:

	ActionPluginInterface() :
			PluginInterface() {
	}
	virtual ~ActionPluginInterface();

	/**
	 * @brief stored in ProgramCtx, accessed using getPluginEnvironment<ActionAtom>()
	 */
	class Environment: public PluginEnvironment {
	public:
		Environment();
		virtual ~Environment() {
		}
		;
	};

	// CRTP pattern
	template<class UserPlugin>
	/**
	 * @brief Base class to implement an External Atom
	 */
	class PluginActionAtom: public PluginAtom {
	public:
		/**
		 * @brief the Environment of this Plugin
		 */
		typedef typename UserPlugin::Environment Environment;

		PluginActionAtom(std::string name) :
				PluginAtom(name, false) {
			prop.pa = this;
			prop.usesEnvironment = true;
		}

		/**
		 * @brief The function that will be called by dlvhex to evaluate the External Atom
		 */
		void retrieve(const Query& query, Answer& answer) {
			const typename UserPlugin::Environment& environment =
					query.ctx->getPluginEnvironment<UserPlugin>();
			retrieve(environment, query, answer);
		}
	protected:
		/**
		 * @brief The function that must be overridden by the creator of the External Atom to execute the own code
		 */
		virtual void retrieve(const typename UserPlugin::Environment&,
				const Query&, Answer&) = 0;
	};

	// CRTP pattern
	template<class UserPlugin>
	/**
	 * @brief Base class to implement an Action
	 */
	class PluginAction: public PluginActionBase {
	public:
		/**
		 * @brief the Environment of this Plugin
		 */
		typedef typename UserPlugin::Environment Environment;

		PluginAction(const std::string& predicate) :
				PluginActionBase(predicate) {
		}

		/**
		 * @brief The function that will be called by the FinalCallback to execute the Actions
		 */
		void execute(ProgramCtx& ctx,
				const InterpretationConstPtr interpretationConstPtr,
				const Tuple& tuple) {
			typename UserPlugin::Environment& environment =
					ctx.getPluginEnvironment<UserPlugin>();
			execute(environment, ctx.registry(), tuple, interpretationConstPtr);
		}
	protected:
		/**
		 * @brief The function that must be overridden by the creator of the Action to execute the own code
		 */
		virtual void execute(typename UserPlugin::Environment&,
				const RegistryPtr, const Tuple&,
				const InterpretationConstPtr) = 0;
	};

	/**
	 * \brief Publish external computation sources to dlvhex.
	 *
	 * This is the central location where the user's atoms are made public.
	 * dlvhex will call this function for all found plugins, which write their
	 * atoms in the provided map. This map associates strings with pointers to
	 * PluginAtom objects. The strings denote the name of the atom as it should
	 * be used in the program.
	 *
	 * Override this method to publish your atoms.
	 *
	 * Example:
	 *
	 * \code
	 *   std::vector<PluginAtomPtr>
	 *   createAtoms(ProgramCtx& ctx) const
	 *   {
	 *    std::vector<PluginAtomPtr> ret;
	 *      PluginAtomPtr newatom1(new MyAtom1);
	 *      PluginAtomPtr newatom2(new MyAtom2);
	 *      ret["newatom1"] = newatom1;
	 *      ret["newatom2"] = newatom2;
	 *      return ret;
	 *   }
	 * \endcode
	 *
	 * Here, we assume to have defined atoms MyAtom1 and MyAtom2
	 * derived from PluginAtom. These atom can now be used in a
	 * HEX-program with the predicate \b &newatom1[]() and
	 * \b &newatom2[]().
	 */
	virtual std::vector<PluginAtomPtr> createAtoms(ProgramCtx& ctx) const {
		std::vector < PluginAtomPtr > allPluginAtoms;
		return allPluginAtoms;
	}

	/**
	 * @brief Publish Actions to ActionPlugin
	 */
	virtual std::vector<PluginActionBasePtr> createActions(
			ProgramCtx& ctx) const {
		std::vector<PluginActionBasePtr> allPluginActions;
		return allPluginActions;
	}

	/**
	 * @brief Will be called by ActionPlugin to collect the BestModelSelectors
	 */
	virtual std::vector<BestModelSelectorPtr> getAllBestModelSelectors() const {
		std::vector<BestModelSelectorPtr> allBestModelSelectors;
		return allBestModelSelectors;
	}

	/**
	 * @brief Will be called by ActionPlugin to collect the ExecutionScheduleBuilders
	 */
	virtual std::vector<ExecutionScheduleBuilderPtr> getAllExecutionScheduleBuilders() const {
		std::vector<ExecutionScheduleBuilderPtr> allExecutionScheduleBuilders;
		return allExecutionScheduleBuilders;
	}

	/**
	 * @brief Used to activate the Plugin only if "--action-enable" option is selected
	 */
	virtual void processOptions(std::list<const char*>& pluginOptions,
			ProgramCtx& ctx);

protected:
	/**
	 * @brief Must be overridden by the Plugins to instantiate themselves
	 */
	virtual ActionPluginInterfacePtr create(ProgramCtx& ctx) {
		throw PluginError("ERROR you haven't overridden create() function");
	}

};

DLVHEX_NAMESPACE_END

#endif /* ACTION_PLUGIN_INTERFACE_H_ */
