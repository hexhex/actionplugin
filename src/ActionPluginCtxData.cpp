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

#include <boost/date_time/posix_time/time_parsers.hpp>

DLVHEX_NAMESPACE_BEGIN

ActionPluginCtxData::ActionPluginCtxData() :
		enabled(false), idActionMap(), levelsAndWeightsBestModels(), bestModelsContainer(), notBestModelsContainer(), iteratorBestModel(), namePluginActionBaseMap(), iterationType(
				DEFAULT), continueIteration(false), stopIteration(false), numberIterations(
				-1), timeDuration(boost::posix_time::not_a_date_time),iterationFromBuiltInConstant(false), nameBestModelSelectorMap(), nameExecutionScheduleBuilderMap() {
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

	for (std::vector<PluginActionBasePtr>::iterator it =
			pluginActionBasePtrVector.begin();
			it != pluginActionBasePtrVector.end(); it++) {

		const ID id = reg->storeConstantTerm((*it)->getPredicate());

		namePluginActionBaseMap.insert(
				std::pair<std::string, PluginActionBasePtr>(
						(*it)->getPredicate(), (*it)));

		std::cerr << "Inserted: " << (*it)->getPredicate() << std::endl;

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

	for (std::vector<BestModelSelectorPtr>::iterator it =
			allBestModelSelectors.begin(); it != allBestModelSelectors.end();
			it++) {
		nameBestModelSelectorMap.insert(
				std::pair<std::string, BestModelSelectorPtr>((*it)->getName(),
						(*it)));
		std::cerr << "Inserted: " << (*it)->getName() << std::endl;
	}
}

// Utility functions used to register ExecutionScheduleBuilders of a Plugin
void ActionPluginCtxData::registerExecutionScheduleBuildersOfPlugin(
		std::vector<ExecutionScheduleBuilderPtr> allExecutionScheduleBuilders) {

	std::cerr << "\nregisterExecutionScheduleBuildersOfPlugin" << std::endl;

	for (std::vector<ExecutionScheduleBuilderPtr>::iterator it =
			allExecutionScheduleBuilders.begin();
			it != allExecutionScheduleBuilders.end(); it++) {
		nameExecutionScheduleBuilderMap.insert(
				std::pair<std::string, ExecutionScheduleBuilderPtr>(
						(*it)->getName(), (*it)));
		std::cerr << "Inserted: " << (*it)->getName() << std::endl;
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

//// function that set the Duration of Iterations
//// it's called when we find the command line option --acthexDurationIterations
//// or a built in constant #acthexDurationIterations
//void ActionPluginCtxData::addDurationIterations(
//		const std::string & string_of_duration, bool fromBuiltInConstant) {
//
//	if (fromBuiltInConstant && !iterationFromBuiltInConstant) {
//
//		if (numberIterations != -1)
//			if (!iterationFromBuiltInConstant)
//				numberIterations = -1;
//			else
//				throw PluginError(
//						"Duplicate values for acthexNumberIterations");
//
//		if (!timeDuration.is_not_a_date_time())
//			if (iterationFromBuiltInConstant)
//				throw PluginError(
//						"Duplicate values for acthexDurationIterations");
//
//		iterationFromBuiltInConstant = true;
//
//	} else if (!timeDuration.is_not_a_date_time())
//		throw PluginError("Duplicate values for acthexDurationIterations");
//
//	if (string_of_duration.length() == 0)
//		return;
//	if (string_of_duration[0] == '-')
//		return;
//
//	if (string_of_duration == "0")
//		iterationType = INFINITE;
//	else {
//		iterationType = FIXED;
//		timeDuration = boost::posix_time::duration_from_string(
//				string_of_duration);
//		startingTime = boost::posix_time::second_clock::local_time();
//	}
//
//}

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

DLVHEX_NAMESPACE_END
