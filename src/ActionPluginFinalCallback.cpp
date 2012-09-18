/**
 * @file ActionPluginFinalCallback.cpp
 * @author Stefano Germano
 *
 * @brief A custom implementation of FinalCallback;
 * will be used for:
 *  - select the BestModel
 *  - build the Execution Scheduler
 *  - execute the Actions in the Execution Scheduler
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H
#include "acthex/ActionPluginFinalCallback.h"

#include "acthex/ActionPlugin.h"
#include "acthex/ActionScheduler.h"
#include "acthex/PluginActionBase.h"
#include "acthex/BestModelSelector.h"
#include "acthex/ExecutionModeRewriter.h"

DLVHEX_NAMESPACE_BEGIN

ActionPluginFinalCallback::ActionPluginFinalCallback(ProgramCtx& ctx) :
		programCtx(ctx), ctxData(ctx.getPluginData<ActionPlugin>()), registryPtr(
				ctx.registry()) {
}

void ActionPluginFinalCallback::operator()() {
	std::cerr << "\nActionPluginFinalCallback called" << std::endl;

#warning I ve to call the BestModelSelection function of the specified ActionAtom
	ctxData.nameBestModelSelectorMap["default"]->getBestModel(
			ctxData.iteratorBestModel, ctxData.bestModelsContainer);

	std::cerr << "\nBestModel selected:" << std::endl;
	(*ctxData.iteratorBestModel)->interpretation->print(std::cerr);
	std::cerr << std::endl;

	std::cerr << "\nCall the executionModeController" << std::endl;
	ActionScheduler* scheduler = new ActionScheduler(ctxData, registryPtr);
	std::multimap<int, Tuple> multimapOfExecution;
	scheduler->executionModeController(multimapOfExecution);

	std::cerr << "\nThe MultiMapOfExecution:" << std::endl;
	std::cerr << "Precedence\tTuple" << std::endl;
	std::multimap<int, Tuple>::iterator itMMOE;
	for (itMMOE = multimapOfExecution.begin();
			itMMOE != multimapOfExecution.end(); itMMOE++) {
		std::cerr << itMMOE->first << "\t\t";
		const Tuple& tempTuple = itMMOE->second;
		ActionScheduler::printTuple(tempTuple, registryPtr);
	}

#warning I ve to call the executionModeRewriter function of the specified ActionAtom
	std::cerr << "\nCall the executionModeRewriter" << std::endl;
	std::list < std::set<Tuple> > listOfExecution;
	ctxData.nameExecutionModeRewriterMap["default"]->rewrite(
			multimapOfExecution, listOfExecution);

	std::cerr << "\nThe ListOfExecution:" << std::endl;
	std::list<std::set<Tuple> >::iterator itLOE;
	for (itLOE = listOfExecution.begin(); itLOE != listOfExecution.end();
			itLOE++) {
		std::set < Tuple > &tempSet = (*itLOE);
		for (std::set<Tuple>::iterator itLOEs = tempSet.begin();
				itLOEs != tempSet.end(); itLOEs++) {
			const Tuple& tempTuple = (*itLOEs);
			ActionScheduler::printTuple(tempTuple, registryPtr);
		}
	}

	std::cerr
			<< "\nControl if the order of Set in the List corresponds to their precedence"
			<< std::endl;
	if (scheduler->checkIfTheListIsCorrect(multimapOfExecution,
			listOfExecution))
		std::cerr << "The List is correct" << std::endl;
	else {
		std::cerr << "The List isn't correct" << std::endl;
		throw PluginError(
				"The order of Set in the ListOfExecution doens't correspond to their precedence");
	}

	std::cerr << "\nExecute the actions in the right order" << std::endl;

	for (itLOE = listOfExecution.begin(); itLOE != listOfExecution.end();
			itLOE++) {

		std::set < Tuple > &tempSet = (*itLOE);

		for (std::set<Tuple>::iterator itLOEs = tempSet.begin();
				itLOEs != tempSet.end(); itLOEs++) {

			const Tuple& tempTuple = (*itLOEs);

			if (*tempTuple.begin() == ctxData.id_continue) {
				ctxData.continueIteration = true;
				continue;
			} else if (*tempTuple.begin() == ctxData.id_stop) {
				ctxData.stopIteration = true;
				continue;
			}

			Tuple tupleForExecute;
			tupleForExecute.insert(tupleForExecute.begin(),
					tempTuple.begin() + 1, tempTuple.end());

			std::cerr << "tempTuple: ";
			ActionScheduler::printTuple(tempTuple, registryPtr);
			std::cerr << "tupleForExecute: ";
			ActionScheduler::printTuple(tupleForExecute, registryPtr);

			std::map<std::string, PluginActionBasePtr>::iterator it =
					ctxData.namePluginActionBaseMap.find(
							registryPtr->getTermStringByID(*tempTuple.begin()));

			if (it != ctxData.namePluginActionBaseMap.end())
				it->second->execute(programCtx,
						(*(ctxData.iteratorBestModel))->interpretation,
						tupleForExecute);
			else
				std::cerr << "For the action '"
						<< registryPtr->getTermStringByID(*tempTuple.begin())
						<< "' wasn't found a definition" << std::endl;

		}
	}

	std::cerr << "\nCheck Iteration" << std::endl;
	if (ctxData.iterationType == DEFAULT && ctxData.continueIteration)
		programCtx.config.setOption("RepeatEvaluation", 1);
	else if (ctxData.iterationType != DEFAULT && ctxData.stopIteration)
		programCtx.config.setOption("RepeatEvaluation", 0);
	else if (ctxData.iterationType == INFINITE)
		programCtx.config.setOption("RepeatEvaluation", 1);
	else if (ctxData.iterationType == FIXED) {
		if (!ctxData.timeDuration.is_not_a_date_time()) {
			boost::posix_time::time_duration diff =
					boost::posix_time::second_clock::local_time()
							- ctxData.startingTime;
			if (diff > ctxData.timeDuration)
				programCtx.config.setOption("RepeatEvaluation", 0);
			else if (ctxData.numberIterations != -1) {
				programCtx.config.setOption("RepeatEvaluation",
						ctxData.numberIterations);
				ctxData.numberIterations--;
			} else
				programCtx.config.setOption("RepeatEvaluation", 1);
		}
	}

	if (programCtx.config.getOption("RepeatEvaluation") > 0) {

		std::cerr << "\nClear data structures" << std::endl;
		ctxData.clearDataStructures();

		ctxData.continueIteration = false;
		ctxData.stopIteration = false;

		std::cerr << "\nReset cache" << std::endl;
		programCtx.resetCacheOfPlugins();

	}

}

DLVHEX_NAMESPACE_END
