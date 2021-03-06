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
class ExecutionScheduleBuilder;
typedef boost::shared_ptr<ExecutionScheduleBuilder> ExecutionScheduleBuilderPtr;

// An enum to specify the type of Iteration
enum IterationType {
	DEFAULT, INFINITE, FIXED
};

/**
 * @brief stored in ProgramCtx, accessed using getPluginData<ActionPlugin>()
 */
class ActionPluginCtxData: public PluginData {
public:
	/**
	 * @brief whether plugin is enabled
	 */
	bool enabled;

	/**
	 * @brief for fast detection whether an ID is this plugin's responsitility to display
	 */
	PredicateMask myAuxiliaryPredicateMask;

	inline const ID& getIDBrave() const { return id_brave; }
	inline const ID& getIDCautious() const { return id_cautious; }
	inline const ID& getIDPreferredCautious() const { return id_preferred_cautious; }

	inline const ID& getIDDefaultPrecedence() const { return id_default_precedence; }
	inline const ID& getIDDefaultWeightWithLevel() const { return id_default_weight_with_level; }
	inline const ID& getIDDefaultWeightWithoutLevel() const { return id_default_weight_without_level; }
	inline const ID& getIDDefaultLevelWithWeight() const { return id_default_level_with_weight; }
	inline const ID& getIDDefaultLevelWithoutWeight() const { return id_default_level_without_weight; }

	inline const IterationType& getIterationType() const { return iterationType; }

	inline const ID& getIDContinue() const { return id_continue; }
	inline const ID& getIDStop() const { return id_stop; }

	inline const boost::posix_time::time_duration& getTimeDuration() const { return timeDuration; }

	inline int getNumberIterations() const { return numberIterations; }

	inline void decreaseNumberIterations() { numberIterations--; }

	inline const boost::posix_time::ptime& getStartingTime() const { return startingTime; }

	inline const std::string& getBestModelSelectorSelected() const { return bestModelSelectorSelected; }

	inline const std::string& getExecutionScheduleBuilderSelected() const { return executionScheduleBuilderSelected; }

	/**
	 * @brief store, in the Registry constant Terms for id_brave, id_cautious, id_preferred_cautious
	 */
	void storeConstantTermsForActionOptions(RegistryPtr);

	/**
	 * @brief initialize id_default_precedence, id_default_weight_with_level, id_default_weight_without_level, id_default_level_with_weight and id_default_level_without_weight
	 */
	void initializeDefaultValuesForPrecedenceWeightAndLevel();

	/**
	 * @brief a map that stores for each ID the corresponding action
	 */
	typedef std::map<ID, ActionPtr> IDActionMap;
	IDActionMap idActionMap;

	typedef std::map<int, int> LevelsAndWeights;
	/**
	 * @brief a map that contains for each level the weight
	 */
	LevelsAndWeights levelsAndWeightsBestModels;

	typedef std::list<AnswerSetPtr> BestModelsContainer;
	/**
	 * @brief the AnswerSets that are Best Models (which have, as weight for each level, the value stored in levelsAndWeightsBestModels)
	 */
	BestModelsContainer bestModelsContainer;

	/**
	 * @brief the AnswerSets that aren't Best Models
	 */
	BestModelsContainer notBestModelsContainer;

	/**
	 * @brief an iterator that identifies the position of the BestModel in BestModelsContainer
	 */
	BestModelsContainer::const_iterator iteratorBestModel;

	ActionPluginCtxData();

	virtual ~ActionPluginCtxData();

	/**
	 * @brief add Actions to idActionMap and myAuxiliaryPredicateMask
	 */
	void addAction(const ID &, const ActionPtr);

	typedef std::map<std::string, PluginActionBasePtr> NamePluginActionBaseMap;
	/**
	 * @brief a map that contains the name and a pointer to the corresponding Action
	 */
	NamePluginActionBaseMap namePluginActionBaseMap;

	/**
	 * @brief called by Actions to register themselves
	 */
	void registerPlugin(ActionPluginInterfacePtr, ProgramCtx&);

	/**
	 * @brief makes the ActionPlugin ready to execute another iteration
	 */
	void clearDataStructures();

	/**
	 * @brief if we have to execute another Iteration
	 */
	bool continueIteration;

	/**
	 * @brief if we have to stop the Iterations
	 */
	bool stopIteration;

	/**
	 * @brief creates the Actions "#continueIteration" and "#stopIteration"
	 */
	void createAndInsertContinueAndStopActions(RegistryPtr);

	typedef std::map<std::string, BestModelSelectorPtr> NameBestModelSelectorMap;
	/**
	 * @brief a map that contains the name and a pointer to the corresponding BestModelSelector
	 */
	NameBestModelSelectorMap nameBestModelSelectorMap;

	typedef std::map<std::string, ExecutionScheduleBuilderPtr> NameExecutionScheduleBuilderMap;
	/**
	 * @brief a map that contains the name and a pointer to the corresponding ExecutionScheduleBuilder
	 */
	NameExecutionScheduleBuilderMap nameExecutionScheduleBuilderMap;

	/**
	 * @brief sets the Number of Iterations
	 *
	 * it's called when we find the command line option --acthexNumberIterations or a built in constant #acthexNumberIterations
	 */
	void addNumberIterations(const unsigned int, ProgramCtx&, bool);
	/**
	 * @brief sets the Duration of Iterations
	 *
	 * it's called when we find the command line option --acthexDurationIterations or a built in constant #acthexDurationIterations
	 */
	void addDurationIterations(unsigned int, bool);

	/**
	 * @brief if we changed the value of Iteration from Built-in Constant
	 */
	bool iterationFromBuiltInConstant;

	/**
	 * @brief inserts DefaultBestModelSelector in nameBestModelSelectorMap and DefaultExecutionScheduleBuilder in nameExecutionScheduleBuilderMap
	 */
	void insertDefaultBestModelSelectorAndDefaultExecutionScheduleBuilder();

	/**
	 * @brief sets the bestModelSelectorSelected
	 */
	void setBestModelSelectorSelected(const std::string);

	/**
	 * @brief sets the executionScheduleBuilderSelected
	 */
	void setExecutionScheduleBuilderSelected(const std::string);

	/**
	 * @brief increase the value of iteration in the UtilitiesPlugin Environment
	 */
	void increaseIteration(ProgramCtx& ctx);

protected:

	/**
	 * @brief ids stored in Registry that represent the action options
	 */
	ID id_brave;
	ID id_cautious;
	ID id_preferred_cautious;

	/**
	 * @brief ids stored in Registry that represent the default values for precedence, weight and level
	 */
	ID id_default_precedence;
	ID id_default_weight_with_level;
	ID id_default_weight_without_level;
	ID id_default_level_with_weight;
	ID id_default_level_without_weight;

	/**
	 * @brief the type of Iteration (an enum)
	 */
	IterationType iterationType;

	/**
	 * @brief ids stored in Registry that represent the Actions "#continueIteration" and "#stopIteration"
	 */
	ID id_continue;
	ID id_stop;

	/**
	 * @brief the time after which the Iterations must stop
	 */
	boost::posix_time::time_duration timeDuration;

	/**
	 * @brief a integer that specify the number of Iterations; will be used only if both (number and time) are specified
	 */
	int numberIterations;

	/**
	 * @brief the time when the ActionPlugin starts
	 */
	boost::posix_time::ptime startingTime;

	/**
	 * @brief the name of BestModelSelector that will be used to select the BestModelSelector from NameBestModelSelectorMap
	 */
	std::string bestModelSelectorSelected;

	/**
	 * @brief the name of ExecutionScheduleBuilder that will be used to select the ExecutionScheduleBuilder from NameExecutionScheduleBuilderMap
	 */
	std::string executionScheduleBuilderSelected;

	/**
	 * @brief Utility functions used to register all parts of a Plugin of the ActionPlugin
	 */
	void registerActionsOfPlugin(std::vector<PluginActionBasePtr>, RegistryPtr);
	void registerBestModelSelectorsOfPlugin(std::vector<BestModelSelectorPtr>);
	void registerExecutionScheduleBuildersOfPlugin(std::vector<ExecutionScheduleBuilderPtr>);

};

DLVHEX_NAMESPACE_END

#endif /*ACTION_PLUGIN_CTX_DATA_H*/
