/**
 * @file ActionPluginCtxData.h
 * @author Stefano Germano
 *
 * @brief CtxData of ActionPlugin
 */

#ifndef ACTION_PLUGIN_CTX_DATA_H
#define ACTION_PLUGIN_CTX_DATA_H

#include "acthex/Action.h"

#include "dlvhex2/PluginInterface.h"
#include "dlvhex2/ProgramCtx.h"
#include "dlvhex2/Registry.h"
#include "dlvhex2/Printer.h"

DLVHEX_NAMESPACE_BEGIN

//Forward declarations
class ActionPluginInterface;
typedef boost::shared_ptr<ActionPluginInterface> ActionPluginInterfacePtr;
#warning could we put PluginActionBase in ActionPluginInterface?
class PluginActionBase;
typedef boost::shared_ptr<PluginActionBase> PluginActionBasePtr;
class BestModelSelector;
typedef boost::shared_ptr<BestModelSelector> BestModelSelectorPtr;
class ExecutionModeRewriter;
typedef boost::shared_ptr<ExecutionModeRewriter> ExecutionModeRewriterPtr;

// An enum to specify the type of Iteration
enum IterationType {
	DEFAULT, INFINITE, FIXED
};

// stored in ProgramCtx, accessed using getPluginData<ActionPlugin>()
class ActionPluginCtxData: public PluginData {
public:
	// whether plugin is enabled
	bool enabled;

	// for fast detection whether an ID is this plugin's responsitility to display
	PredicateMask myAuxiliaryPredicateMask;

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

	ActionPluginCtxData();

	virtual ~ActionPluginCtxData();

	// add Actions to idActionMap and myAuxiliaryPredicateMask
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

	// a integer that specify the number of Iterations
	// will be used only if both (number and time) are specified
	int numberIterations;

	// the time after which the Iterations must stop
	boost::posix_time::time_duration timeDuration;

	// the time when the ActionPlugin starts
	boost::posix_time::ptime startingTime;

	typedef std::map<std::string, BestModelSelectorPtr> NameBestModelSelectorMap;
	// a map that contains the name and a pointer to the corresponding BestModelSelector
	NameBestModelSelectorMap nameBestModelSelectorMap;

	typedef std::map<std::string, ExecutionModeRewriterPtr> NameExecutionModeRewriterMap;
	// a map that contains the name and a pointer to the corresponding ExecutionModeRewriter
	NameExecutionModeRewriterMap nameExecutionModeRewriterMap;

	// function that set the Number of Iterations
	// it's called when we find the command line option --acthexNumberIterations
	// or a built in constant #acthexNumberIterations
	void addNumberIterations(const unsigned int, ProgramCtx&, bool);
	// function that set the Duration of Iterations
	// it's called when we find the command line option --acthexDurationIterations
	// or a built in constant #acthexDurationIterations
//	void addDurationIterations(const std::string &, bool);
	void addDurationIterations(unsigned int, bool);

	// if we changed the value of Iteration from Built-in Constant
	bool iterationFromBuiltInConstant;

private:
	// Utility functions used to register all parts of a Plugin of the ActionPlugin
	void registerActionsOfPlugin(std::vector<PluginActionBasePtr>, RegistryPtr);
	void registerBestModelSelectorsOfPlugin(std::vector<BestModelSelectorPtr>);
	void registerExecutionModeRewritersOfPlugin(
			std::vector<ExecutionModeRewriterPtr>);
};

DLVHEX_NAMESPACE_END

#endif /*ACTION_PLUGIN_CTX_DATA_H*/
