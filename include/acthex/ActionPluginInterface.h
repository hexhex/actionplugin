/**
 * @file ActionPluginInterface.h
 * @author Stefano Germano
 *
 * @brief ...
 */

#ifndef ACTION_PLUGIN_INTERFACE_H_
#define ACTION_PLUGIN_INTERFACE_H_

#include "acthex/ActionPlugin.h"
#include "acthex/PluginActionBase.h"

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
	class PluginActionAtom: public PluginAtom {
	public:

		typedef typename Derived::Environment Environment;

		PluginActionAtom(std::string name) :
				PluginAtom(name, false) {
		}

		void retrieve(const Query& query, Answer& answer) {
			std::cerr << "retrive" << std::endl;
			const typename Derived::Environment& environment =
					query.ctx->getPluginEnvironment<Derived>();
			retrieve(environment, query, answer);
		}

	protected:
		virtual void retrieve(const typename Derived::Environment&,
				const Query&, Answer&) = 0;
	};

	// CRTP pattern
	template<class Derived>
	class PluginAction: public PluginActionBase {
	public:

		typedef typename Derived::Environment Environment;

		PluginAction(const std::string& predicate) :
				PluginActionBase(predicate) {
		}

		void execute(ProgramCtx& ctx, const Tuple& tuple) {
#warning I don t know I can have InterpretationConstPtr;
			InterpretationConstPtr interpretationConstPtr;
			typename Derived::Environment& environment =
					ctx.getPluginEnvironment<Derived>();
			execute(environment, ctx.registry(), tuple, interpretationConstPtr);
		}

	protected:
		virtual void execute(typename Derived::Environment&,
				const RegistryPtr registryPtr, const Tuple&,
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
	virtual std::vector<PluginAtomPtr> createAtoms(ProgramCtx& ctx) const = 0;

	virtual std::vector<PluginActionBasePtr> createActions(
			ProgramCtx& ctx) const = 0;

	class BestModelSelector {
		BestModelSelector(std::string name) :
				name(name) {
		}
		;
		virtual void getBestModel(
				dlvhex::ActionPlugin::CtxData::BestModelsContainer::iterator&,
				const dlvhex::ActionPlugin::CtxData::BestModelsContainer&) = 0;
	protected:
		std::string name;
	};
	typedef boost::shared_ptr<BestModelSelector> BestModelSelectorPtr;

	virtual std::vector<BestModelSelectorPtr> getAllBestModelSelectors() const {
		return std::vector<BestModelSelectorPtr>();
	}

	class ExecutionModeRewriter {
		ExecutionModeRewriter(std::string name) :
				name(name) {
		}
		;
		virtual void rewrite(
				const std::multimap<int, Tuple>& multimapOfExecution,
				std::list<std::set<Tuple> >& listOfExecution) = 0;
	protected:
		std::string name;
	};
	typedef boost::shared_ptr<ExecutionModeRewriter> ExecutionModeRewriterPtr;

	virtual std::vector<ExecutionModeRewriterPtr> getAllExecutionModeRewriters() const {
		return std::vector<ExecutionModeRewriterPtr>();
	}

	virtual void processOptions(std::list<const char*>& pluginOptions,
			ProgramCtx& ctx) {
		std::cerr << "processOptions of ActionPluginInterface" << std::endl;
		registerInActionPlugin(ctx);
	}

	virtual void setupProgramCtx(ProgramCtx&) {
	}

protected:
	void registerInActionPlugin(ProgramCtx& ctx) {
		std::cerr << "registerInActionPlugin of ActionPluginInterface"
				<< std::endl;

//		boost::shared_ptr<ActionPluginInterface> pointer_shared_ptr = boost::shared_ptr<ActionPluginInterface>(this);
//		std::cerr << "after shared_ptr" << std::endl;

//		boost::shared_ptr<ActionPluginInterface> pointer_shared_from_this =
//				shared_from_this();
//		std::cerr << "after shared_from_this" << std::endl;

//		ctx.getPluginData<ActionPlugin>().registerPlugin(shared_from_this(), ctx);
		ctx.getPluginData<ActionPlugin>().registerPlugin(this->create(ctx), ctx);
	}

	virtual ActionPluginInterfacePtr create(ProgramCtx& ctx) {
		throw PluginError("ERROR you haven't overridden create() function");
	}

};
typedef boost::shared_ptr<ActionPluginInterface> ActionPluginInterfacePtr;

DLVHEX_NAMESPACE_END

#endif /* ACTION_PLUGIN_INTERFACE_H_ */
