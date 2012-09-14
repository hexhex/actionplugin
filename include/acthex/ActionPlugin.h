/* dlvhex -- Answer-Set Programming with external interfaces.
 * Copyright (C) 2005, 2006, 2007 Roman Schindlauer
 * Copyright (C) 2006, 2007, 2008, 2009, 2010, 2011 Thomas Krennwallner
 * Copyright (C) 2009, 2010, 2011 Peter Schüller
 * 
 * This file is part of dlvhex.
 *
 * dlvhex is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * dlvhex is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with dlvhex; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA.
 */

/**
 * @file ActionPlugin.h
 * @author Stefano Germano
 *
 * @brief Plugin ...
 */

#ifndef ACTION_PLUGIN_H
#define ACTION_PLUGIN_H

#include "acthex/Action.h"
//#include "acthex/PluginActionBase.h"

#include "dlvhex2/PlatformDefinitions.h"
#include "dlvhex2/PluginInterface.h"
#include "dlvhex2/HexParserModule.h"
#include "dlvhex2/ProgramCtx.h"
#include "dlvhex2/Registry.h"

DLVHEX_NAMESPACE_BEGIN

//Forward declaration
class ActionPluginInterface;
typedef boost::shared_ptr<ActionPluginInterface> ActionPluginInterfacePtr;
#warning could we put PluginActionBase in ActionPluginInterface?
class PluginActionBase;
typedef boost::shared_ptr<PluginActionBase> PluginActionBasePtr;

class BestModelSelector;
typedef boost::shared_ptr<BestModelSelector> BestModelSelectorPtr;
class ExecutionModeRewriter;
typedef boost::shared_ptr<ExecutionModeRewriter> ExecutionModeRewriterPtr;

enum IterationType {
	DEFAULT, INFINITE, FIXED
};

class ActionPlugin: public PluginInterface {
public:

	// stored in ProgramCtx, accessed using getPluginData<ActionPlugin>()
	class CtxData: public PluginData {
	public:
		// whether plugin is enabled
		bool enabled;

		// for fast detection whether an ID is this plugin's responsitility to display
		PredicateMask myAuxiliaryPredicateMask;

		// an id that is stored in Registry and give the string representing the name of each action atom "rewritten"
//		dlvhex::ID id_in_the_registry;

// ids stored in Registry that represent the action options
		ID id_brave;
		ID id_cautious;
		ID id_preferred_cautious;

		// ids stored in Registry that represent the default values for precedence, weight and level
		ID id_default_precedence;
		ID id_default_weight_with_level;
		ID id_default_weight_without_level;
		ID id_default_level_with_weight;
		ID id_default_level_without_weight;

		// a map that stores for each ID the corresponding action
		typedef std::map<ID, ActionPtr> IDActionMap;
		IDActionMap idActionMap;

		typedef std::map<int, int> LevelsAndWeights;
		// a map that contains for each level the weight
		LevelsAndWeights levelsAndWeightsBestModels;

		typedef std::list<AnswerSetPtr> BestModelsContainer;
		// the AnswerSets that are Best Models (which have, as weight for each level, the value stored in levelsAndWeightsBestModels)
		BestModelsContainer bestModelsContainer;

		// the AnswerSets that aren't Best Models
		BestModelsContainer notBestModelsContainer;

		// an iterator that identifies the position of the BestModel in BestModelsContainer
		BestModelsContainer::const_iterator iteratorBestModel;

		CtxData();

		virtual ~CtxData();

		// add actions to idActionMap and myAuxiliaryPredicateMask
		void addAction(const ID &, const ActionPtr);

		typedef std::map<std::string, PluginActionBasePtr> NamePluginActionBaseMap;
		// a map that contains the name and a pointer to the corresponding Action
		NamePluginActionBaseMap namePluginActionBaseMap;

		// called by Actions to register themselves
		void registerPlugin(ActionPluginInterfacePtr, ProgramCtx&);

		// makes the plugin ready to execute another iteration
		void clearDataStructures();

		// the type of Iteration (an enum)
		IterationType iterationType;

		// if we have to execute another Iteration
		bool continueIteration;

		// if we have to stop the Iterations
		bool stopIteration;

		// ids stored in Registry that represent the Actions "#continueIteration" and "#stopIteration"
		ID id_continue;
		ID id_stop;

		// creates the Actions "#continueIteration" and "#stopIteration"
		void createAndInsertContinueAndStopActions(RegistryPtr);

		int numberIterations;

		boost::posix_time::time_duration timeDuration;

		boost::posix_time::ptime startingTime;

		typedef std::map<std::string, BestModelSelectorPtr> NameBestModelSelectorMap;
		// a map that contains the name and a pointer to the corresponding BestModelSelector
		NameBestModelSelectorMap nameBestModelSelectorMap;

		typedef std::map<std::string, ExecutionModeRewriterPtr> NameExecutionModeRewriterMap;
		// a map that contains the name and a pointer to the corresponding ExecutionModeRewriter
		NameExecutionModeRewriterMap nameExecutionModeRewriterMap;

	private:
		void registerActionsOfPlugin(std::vector<PluginActionBasePtr>,
				RegistryPtr);
		void registerBestModelSelectorsOfPlugin(
				std::vector<BestModelSelectorPtr>);
		void registerExecutionModeRewritersOfPlugin(
				std::vector<ExecutionModeRewriterPtr>);
	};

	ActionPlugin();
	virtual ~ActionPlugin();

	// output help message for this plugin
	virtual void printUsage(std::ostream& o) const;

	// accepted options: --action-enable
	//
	// processes options for this plugin, and removes recognized options from pluginOptions
	// (do not free the pointers, the const char* directly come from argv)
	virtual void processOptions(std::list<const char*>& pluginOptions,
			ProgramCtx&);

	// create parser modules that extend and the basic hex grammar
	virtual std::vector<HexParserModulePtr> createParserModules(ProgramCtx&);

	virtual void setupProgramCtx(ProgramCtx&);

	static void printTuple(const Tuple& tuple, const RegistryPtr registryPtr) {
		bool first = true;
		for (Tuple::const_iterator it = tuple.begin(); it != tuple.end();
				it++) {
			if (first)
				first = !first;
			else
				std::cerr << ", ";
			if (it->isConstantTerm() || it->isVariableTerm())
				std::cerr << registryPtr->getTermStringByID(*it);
			else
				std::cerr << it->address;
		}
		std::cerr << std::endl;
	}
	;

};
typedef boost::shared_ptr<ActionPlugin::CtxData> CtxDataPtr;

DLVHEX_NAMESPACE_END

#endif
