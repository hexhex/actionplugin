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

// add Actions to idActionMap and myAuxiliaryPredicateMask
void ActionPluginCtxData::addAction(const ID & id, const ActionPtr actionPtr) {

	idActionMap.insert(std::pair<ID, ActionPtr>(id, actionPtr));

	myAuxiliaryPredicateMask.addPredicate(actionPtr->getAuxId());

}

// called by Actions to register themselves
void ActionPluginCtxData::registerPlugin(
		ActionPluginInterfacePtr actionPluginInterfacePtr, ProgramCtx& ctx) {

	std::cerr << "registerPlugin called" << std::endl;

	registerActionsOfPlugin(actionPluginInterfacePtr->createActions(ctx),
			ctx.registry());

	registerBestModelSelectorsOfPlugin(
			actionPluginInterfacePtr->getAllBestModelSelectors());

	registerExecutionScheduleBuildersOfPlugin(
			actionPluginInterfacePtr->getAllExecutionScheduleBuilders());

}

// Utility functions used to register Actions of a Plugin
void ActionPluginCtxData::registerActionsOfPlugin(
		std::vector<PluginActionBasePtr> pluginActionBasePtrVector,
		RegistryPtr reg) {

	std::cerr << "\nregisterActionsOfPlugin" << std::endl;

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

		std::cerr << "Inserted: " << actionPredicate << std::endl;

		ID aux_id = reg->getAuxiliaryConstantSymbol('a', id);

		RawPrinter printer(std::cerr, reg);

		std::cerr << "Id: ";
		printer.print(id);
		std::cerr << "\t";
		printer.print(aux_id);
		std::cerr << std::endl;

		ActionPtr actionPtr(new Action(reg->getTermStringByID(id), aux_id));
		this->addAction(id, actionPtr);

	}

}

// Utility functions used to register BestModelSelectors of a Plugin
void ActionPluginCtxData::registerBestModelSelectorsOfPlugin(
		std::vector<BestModelSelectorPtr> allBestModelSelectors) {

	std::cerr << "\nregisterBestModelSelectorsOfPlugin" << std::endl;

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

		std::cerr << "Inserted: " << bestModelSelectorName << std::endl;

	}
}

// Utility functions used to register ExecutionScheduleBuilders of a Plugin
void ActionPluginCtxData::registerExecutionScheduleBuildersOfPlugin(
		std::vector<ExecutionScheduleBuilderPtr> allExecutionScheduleBuilders) {

	std::cerr << "\nregisterExecutionScheduleBuildersOfPlugin" << std::endl;

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

		std::cerr << "Inserted: " << executionScheduleBuilderName << std::endl;

	}

}

// makes the plugin ready to execute another iteration
void ActionPluginCtxData::clearDataStructures() {
	levelsAndWeightsBestModels.clear();
	bestModelsContainer.clear();
	notBestModelsContainer.clear();
	iteratorBestModel = bestModelsContainer.end();
}

// creates the Actions "#continueIteration" and "#stopIteration"
void ActionPluginCtxData::createAndInsertContinueAndStopActions(
		RegistryPtr reg) {

	id_continue = reg->storeConstantTerm("acthexContinue");
	id_stop = reg->storeConstantTerm("acthexStop");

	RawPrinter printer(std::cerr, reg);

	ID aux_id_continue = reg->getAuxiliaryConstantSymbol('a', id_continue);
	ActionPtr actionPtrContinue(
			new Action(reg->getTermStringByID(id_continue), aux_id_continue));
	this->addAction(id_continue, actionPtrContinue);

	std::cerr << "id_continue : ";
	printer.print(id_continue);
	std::cerr << ",\t aux_id_continue: ";
	printer.print(aux_id_continue);
	std::cerr << std::endl;

	ID aux_id_stop = reg->getAuxiliaryConstantSymbol('a', id_stop);
	ActionPtr actionPtrStop(
			new Action(reg->getTermStringByID(id_stop), aux_id_stop));
	this->addAction(id_stop, actionPtrStop);

	std::cerr << "id_stop : ";
	printer.print(id_stop);
	std::cerr << ",\t aux_id_stop:";
	printer.print(aux_id_stop);
	std::cerr << std::endl;

}

// function that set the Number of Iterations
// it's called when we find the command line option --acthexNumberIterations
// or a built in constant #acthexNumberIterations
void ActionPluginCtxData::addNumberIterations(const unsigned int number,
		ProgramCtx& ctx, bool fromBuiltInConstant) {

	if (fromBuiltInConstant && !iterationFromBuiltInConstant) {

		timeDuration = boost::posix_time::not_a_date_time;

		iterationFromBuiltInConstant = true;

	} else if (numberIterations != -1)
		throw PluginError("Duplicate values for acthexNumberIterations");

	if (number == 0)
		iterationType = INFINITE;
	else {
		iterationType = FIXED;
		ctx.config.setOption("RepeatEvaluation", number);
		numberIterations = number;
	}

}

// function that set the Duration of Iterations
// it's called when we find the command line option --acthexDurationIterations
// or a built in constant #acthexDurationIterations
void ActionPluginCtxData::addDurationIterations(unsigned int duration,
		bool fromBuiltInConstant) {

	if (fromBuiltInConstant && !iterationFromBuiltInConstant) {

		numberIterations = -1;

		iterationFromBuiltInConstant = true;

	} else if (!timeDuration.is_not_a_date_time())
		throw PluginError("Duplicate values for acthexDurationIterations");

	if (duration == 0)
		iterationType = INFINITE;
	else {
		iterationType = FIXED;
		timeDuration = boost::posix_time::seconds(duration);
		startingTime = boost::posix_time::second_clock::local_time();
	}

}

//insert DefaultBestModelSelector in nameBestModelSelectorMap
// and DefaultExecutionScheduleBuilder in nameExecutionScheduleBuilderMap
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

// Set the bestModelSelectorSelected
void ActionPluginCtxData::setBestModelSelectorSelected(const std::string bestModelSelector) {

	if(bestModelSelectorSelected != "default")
		throw PluginError("Duplicate values for acthexBestModelSelector");

	bestModelSelectorSelected = bestModelSelector;

}

//Set the executionScheduleBuilderSelected
void ActionPluginCtxData::setExecutionScheduleBuilderSelected(const std::string executionScheduleBuilder) {

	if(executionScheduleBuilder != "default")
		throw PluginError("Duplicate values for acthexExecutionScheduleBuilder");

	executionScheduleBuilderSelected = executionScheduleBuilder;

}

DLVHEX_NAMESPACE_END
