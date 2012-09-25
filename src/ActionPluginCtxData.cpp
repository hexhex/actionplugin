/**
 * @file ActionPluginCtxData.cpp
 * @author Stefano Germano
 *
 * @brief CtxData of ActionPlugin
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H
#include "acthex/ActionPluginCtxData.h"

#include "acthex/ActionPluginInterface.h"
#include "acthex/DefaultBestModelSelector.h"
#include "acthex/DefaultExecutionScheduleBuilder.h"

#include <boost/date_time/posix_time/time_parsers.hpp>

DLVHEX_NAMESPACE_BEGIN

ActionPluginCtxData::ActionPluginCtxData() :
		enabled(false), idActionMap(), levelsAndWeightsBestModels(), bestModelsContainer(), notBestModelsContainer(), iteratorBestModel(), namePluginActionBaseMap(), iterationType(
				DEFAULT), continueIteration(false), stopIteration(false), numberIterations(
				-1), timeDuration(boost::posix_time::not_a_date_time), iterationFromBuiltInConstant(
				false), nameBestModelSelectorMap(), nameExecutionScheduleBuilderMap(), bestModelSelectorSelected(
				"default"), executionScheduleBuilderSelected("default") {
}

ActionPluginCtxData::~ActionPluginCtxData() {
	idActionMap.clear();
	levelsAndWeightsBestModels.clear();
	bestModelsContainer.clear();
	notBestModelsContainer.clear();
	iteratorBestModel = bestModelsContainer.end();
	namePluginActionBaseMap.clear();
	nameBestModelSelectorMap.clear();
	nameExecutionScheduleBuilderMap.clear();
}

/**
 * @brief add Actions to idActionMap and myAuxiliaryPredicateMask
 */
void ActionPluginCtxData::addAction(const ID & id, const ActionPtr actionPtr) {

	idActionMap.insert(std::pair<ID, ActionPtr>(id, actionPtr));

	myAuxiliaryPredicateMask.addPredicate(actionPtr->getAuxId());

}

/**
 * @brief called by Actions to register themselves
 */
void ActionPluginCtxData::registerPlugin(
		ActionPluginInterfacePtr actionPluginInterfacePtr, ProgramCtx& ctx) {

	DBGLOG(DBG, "registerPlugin called");

	registerActionsOfPlugin(actionPluginInterfacePtr->createActions(ctx),
			ctx.registry());

	registerBestModelSelectorsOfPlugin(
			actionPluginInterfacePtr->getAllBestModelSelectors());

	registerExecutionScheduleBuildersOfPlugin(
			actionPluginInterfacePtr->getAllExecutionScheduleBuilders());

}

/**
 * @brief utility functions used to register Actions of a Plugin
 */
void ActionPluginCtxData::registerActionsOfPlugin(
		std::vector<PluginActionBasePtr> pluginActionBasePtrVector,
		RegistryPtr reg) {

	DBGLOG(DBG, "\nregisterActionsOfPlugin");

	std::string actionPredicate;

	for (std::vector<PluginActionBasePtr>::iterator it =
			pluginActionBasePtrVector.begin();
			it != pluginActionBasePtrVector.end(); it++) {

		actionPredicate = (*it)->getPredicate();

		if (namePluginActionBaseMap.count(actionPredicate) > 0)
			throw PluginError(
					"There are 2 Actions with the same Predicate value");

		const ID id = reg->storeConstantTerm(actionPredicate);

		namePluginActionBaseMap.insert(
				std::pair<std::string, PluginActionBasePtr>(actionPredicate,
						(*it)));

		DBGLOG(DBG, "Inserted: " << actionPredicate);

		ID aux_id = reg->getAuxiliaryConstantSymbol('a', id);

		std::stringstream ss;
		RawPrinter printer(ss, reg);

		printer.print(id);
		ss << "\t";
		printer.print(aux_id);

		DBGLOG(DBG, "Id: " << ss.str());

		ActionPtr actionPtr(new Action(reg->getTermStringByID(id), aux_id));
		this->addAction(id, actionPtr);

	}

}

/**
 * @brief utility functions used to register BestModelSelectors of a Plugin
 */
void ActionPluginCtxData::registerBestModelSelectorsOfPlugin(
		std::vector<BestModelSelectorPtr> allBestModelSelectors) {

	DBGLOG(DBG, "\nregisterBestModelSelectorsOfPlugin");

	std::string bestModelSelectorName;

	for (std::vector<BestModelSelectorPtr>::iterator it =
			allBestModelSelectors.begin(); it != allBestModelSelectors.end();
			it++) {

		bestModelSelectorName = (*it)->getName();

		if (nameBestModelSelectorMap.count(bestModelSelectorName) > 0)
			throw PluginError(
					"There are 2 BestModelSelectors with the same Name value");

		nameBestModelSelectorMap.insert(
				std::pair<std::string, BestModelSelectorPtr>(
						bestModelSelectorName, (*it)));

		DBGLOG(DBG, "Inserted: " << bestModelSelectorName);

	}
}

/**
 * @brief utility functions used to register ExecutionScheduleBuilders of a Plugin
 */
void ActionPluginCtxData::registerExecutionScheduleBuildersOfPlugin(
		std::vector<ExecutionScheduleBuilderPtr> allExecutionScheduleBuilders) {

	DBGLOG(DBG, "\nregisterExecutionScheduleBuildersOfPlugin");

	std::string executionScheduleBuilderName;

	for (std::vector<ExecutionScheduleBuilderPtr>::iterator it =
			allExecutionScheduleBuilders.begin();
			it != allExecutionScheduleBuilders.end(); it++) {

		executionScheduleBuilderName = (*it)->getName();

		if (nameExecutionScheduleBuilderMap.count(executionScheduleBuilderName)
				> 0)
			throw PluginError(
					"There are 2 ExecutionScheduleBuilders with the same Name value");

		nameExecutionScheduleBuilderMap.insert(
				std::pair<std::string, ExecutionScheduleBuilderPtr>(
						executionScheduleBuilderName, (*it)));

		DBGLOG(DBG, "Inserted: " << executionScheduleBuilderName);

	}

}

/**
 * @brief makes the ActionPlugin ready to execute another iteration
 */
void ActionPluginCtxData::clearDataStructures() {
	levelsAndWeightsBestModels.clear();
	bestModelsContainer.clear();
	notBestModelsContainer.clear();
	iteratorBestModel = bestModelsContainer.end();
}

/**
 * @brief creates the Actions "#continueIteration" and "#stopIteration"
 */
void ActionPluginCtxData::createAndInsertContinueAndStopActions(
		RegistryPtr reg) {

	id_continue = reg->storeConstantTerm("acthexContinue");
	id_stop = reg->storeConstantTerm("acthexStop");

	std::stringstream ss;
	RawPrinter printer(ss, reg);

	ID aux_id_continue = reg->getAuxiliaryConstantSymbol('a', id_continue);
	ActionPtr actionPtrContinue(
			new Action(reg->getTermStringByID(id_continue), aux_id_continue));
	this->addAction(id_continue, actionPtrContinue);

	printer.print(id_continue);
	ss << ",\t aux_id_continue: ";
	printer.print(aux_id_continue);

	DBGLOG(DBG, "id_continue : " << ss.str());

	ID aux_id_stop = reg->getAuxiliaryConstantSymbol('a', id_stop);
	ActionPtr actionPtrStop(
			new Action(reg->getTermStringByID(id_stop), aux_id_stop));
	this->addAction(id_stop, actionPtrStop);

	printer.print(id_stop);
	ss << ",\t aux_id_stop:";
	printer.print(aux_id_stop);

	DBGLOG(DBG, "id_stop : " << ss.str());

}

/**
 * @brief sets the Number of Iterations
 *
 * it's called when we find the command line option --acthexNumberIterations or a built in constant #acthexNumberIterations
 */
//
void ActionPluginCtxData::addNumberIterations(const unsigned int number,
		ProgramCtx& ctx, bool fromBuiltInConstant) {

	if (fromBuiltInConstant && !iterationFromBuiltInConstant) {

		timeDuration = boost::posix_time::not_a_date_time;

		iterationFromBuiltInConstant = true;

	} else if (numberIterations != -1)
		if (numberIterations == number)
			return;
		else
			throw PluginError("Duplicate values for acthexNumberIterations");

	if (number == 0)
		iterationType = INFINITE;
	else {
		iterationType = FIXED;
		ctx.config.setOption("RepeatEvaluation", number);
		numberIterations = number;
	}

}

/**
 * @brief sets the Duration of Iterations
 * it's called when we find the command line option --acthexDurationIterations or a built in constant #acthexDurationIterations
 */
void ActionPluginCtxData::addDurationIterations(unsigned int duration,
		bool fromBuiltInConstant) {

	if (fromBuiltInConstant && !iterationFromBuiltInConstant) {

		numberIterations = -1;

		iterationFromBuiltInConstant = true;

	} else if (!timeDuration.is_not_a_date_time())
		if (timeDuration.seconds() == duration)
			return;
		else
			throw PluginError("Duplicate values for acthexDurationIterations");

	if (duration == 0)
		iterationType = INFINITE;
	else {
		iterationType = FIXED;
		timeDuration = boost::posix_time::seconds(duration);
		startingTime = boost::posix_time::second_clock::local_time();
	}

}

/**
 * @brief inserts DefaultBestModelSelector in nameBestModelSelectorMap and DefaultExecutionScheduleBuilder in nameExecutionScheduleBuilderMap
 */
void ActionPluginCtxData::insertDefaultBestModelSelectorAndDefaultExecutionScheduleBuilder() {

	std::vector<BestModelSelectorPtr> defaultBestModelSelectors;
	BestModelSelectorPtr defaultBestModelSelectorPtr(
			new DefaultBestModelSelector("default"));
	defaultBestModelSelectors.push_back(defaultBestModelSelectorPtr);
	registerBestModelSelectorsOfPlugin(defaultBestModelSelectors);

	std::vector<ExecutionScheduleBuilderPtr> defaultExecutionScheduleBuilders;
	ExecutionScheduleBuilderPtr defaultExecutionScheduleBuilderPtr(
			new DefaultExecutionScheduleBuilder("default"));
	defaultExecutionScheduleBuilders.push_back(
			defaultExecutionScheduleBuilderPtr);
	registerExecutionScheduleBuildersOfPlugin(defaultExecutionScheduleBuilders);

}

/**
 * @brief sets the bestModelSelectorSelected
 */
void ActionPluginCtxData::setBestModelSelectorSelected(
		const std::string bestModelSelector) {

	if (bestModelSelectorSelected != "default")
		if (bestModelSelectorSelected == bestModelSelector)
			return;
		else
			throw PluginError("Duplicate values for acthexBestModelSelector");

	bestModelSelectorSelected = bestModelSelector;

}

/**
 * @brief sets the executionScheduleBuilderSelected
 */
void ActionPluginCtxData::setExecutionScheduleBuilderSelected(
		const std::string executionScheduleBuilder) {

	if (executionScheduleBuilder != "default")
		if (executionScheduleBuilderSelected == executionScheduleBuilder)
			return;
		else
			throw PluginError(
					"Duplicate values for acthexExecutionScheduleBuilder");

	executionScheduleBuilderSelected = executionScheduleBuilder;

}

DLVHEX_NAMESPACE_END
