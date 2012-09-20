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

	// stored in ProgramCtx, accessed using getPluginEnvironment<ActionAtom>()
	class Environment: public PluginEnvironment {
	public:
		Environment();
		virtual ~Environment() {
		}
		;
	};

	// CRTP pattern
	template<class Derived>
	// Base class to implement an External Atom
	class PluginActionAtom: public PluginAtom {
	public:
		// the Environment of this Plugin
		typedef typename Derived::Environment Environment;

		PluginActionAtom(std::string name) :
				PluginAtom(name, false) {
			prop.pa = this;
			prop.usesEnvironment = true;
		}

		// The function that will be called by dlvhex to evaluate the External Atom
		void retrieve(const Query& query, Answer& answer) {
			const typename Derived::Environment& environment =
					query.ctx->getPluginEnvironment<Derived>();
			retrieve(environment, query, answer);
		}
	protected:
		// The function that must be overridden by the creator of the External Atom to execute the own code
		virtual void retrieve(const typename Derived::Environment&,
				const Query&, Answer&) = 0;
	};

	// CRTP pattern
	template<class Derived>
	// Base class to implement an Action
	class PluginAction: public PluginActionBase {
	public:
		// the Environment of this Plugin
		typedef typename Derived::Environment Environment;

		PluginAction(const std::string& predicate) :
				PluginActionBase(predicate) {
		}

		// The function that will be called by the FinalCallback to execute the Actions
		void execute(ProgramCtx& ctx,
				const InterpretationConstPtr interpretationConstPtr,
				const Tuple& tuple) {
			typename Derived::Environment& environment =
					ctx.getPluginEnvironment<Derived>();
			execute(environment, ctx.registry(), tuple, interpretationConstPtr);
		}
	protected:
		// The function that must be overridden by the creator of the Action to execute the own code
		virtual void execute(typename Derived::Environment&, const RegistryPtr,
				const Tuple&, const InterpretationConstPtr) = 0;
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
	virtual std::vector<PluginAtomPtr> createAtoms(ProgramCtx& ctx) const = 0;

	// Publish Actions to ActionPlugin
	virtual std::vector<PluginActionBasePtr> createActions(
			ProgramCtx& ctx) const = 0;

	// A default implementation of BestModelSelector
	class DefaultBestModelSelector: public BestModelSelector {
	public:
		DefaultBestModelSelector(std::string name) :
				BestModelSelector(name) {
		}
		virtual void getBestModel(
				ActionPluginCtxData::BestModelsContainer::const_iterator& iteratorBestModel,
				const ActionPluginCtxData::BestModelsContainer& bestModelsContainer) {
			iteratorBestModel = bestModelsContainer.begin();
		}
	};

	// Will be called by ActionPlugin to collect the BestModelSelectors
	virtual std::vector<BestModelSelectorPtr> getAllBestModelSelectors() const {
		std::vector<BestModelSelectorPtr> allBestModelSelectors;
		BestModelSelectorPtr bestModelSelectorPtr(
				new DefaultBestModelSelector("default"));
		allBestModelSelectors.push_back(bestModelSelectorPtr);
		return allBestModelSelectors;
	}

	// A default implementation of ExecutionScheduleBuilder
	class DefaultExecutionScheduleBuilder: public ExecutionScheduleBuilder {
	public:
		DefaultExecutionScheduleBuilder(std::string name) :
			ExecutionScheduleBuilder(name) {
		}
		virtual void rewrite(
				const std::multimap<int, Tuple>& multimapOfExecution,
				std::list<std::set<Tuple> >& listOfExecution) {

			if (multimapOfExecution.empty())
				return;

			std::multimap<int, Tuple>::const_iterator it =
					multimapOfExecution.begin();
			int lastPrecedence = it->first;
			std::set < Tuple > currentSet;
			for (; it != multimapOfExecution.end(); it++) {

//				std::cerr << "lastPrecedence: " << lastPrecedence << std::endl;
//				std::cerr << "in multimap: " << it->first << " "
//						<< registryPtr->getTermStringByID(it->second[0])
//						<< std::endl;

				if (it->first != lastPrecedence) {
					listOfExecution.push_back(currentSet);
					currentSet.clear();
					lastPrecedence = it->first;
				}
				currentSet.insert(it->second);

			}

			listOfExecution.push_back(currentSet);

		}
	};

	// Will be called by ActionPlugin to collect the ExecutionScheduleBuilders
	virtual std::vector<ExecutionScheduleBuilderPtr> getAllExecutionScheduleBuilders() const {
		std::vector<ExecutionScheduleBuilderPtr> allExecutionScheduleBuilders;
		ExecutionScheduleBuilderPtr executionScheduleBuilderPtr(
				new DefaultExecutionScheduleBuilder("default"));
		allExecutionScheduleBuilders.push_back(executionScheduleBuilderPtr);
		return allExecutionScheduleBuilders;
	}

	// Used to activate the Plugin only if "--action-enable" option is selected
	virtual void processOptions(std::list<const char*>& pluginOptions,
			ProgramCtx& ctx);

protected:
	// Must be overridden by the Plugins to instantiate themselves
	virtual ActionPluginInterfacePtr create(ProgramCtx& ctx) {
		throw PluginError("ERROR you haven't overridden create() function");
	}

};

DLVHEX_NAMESPACE_END

#endif /* ACTION_PLUGIN_INTERFACE_H_ */
