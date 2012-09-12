/**
 * @file ActionPluginFinalCallback.cpp
 * @author Stefano Germano
 *
 * @brief ...
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H
#include "acthex/ActionPluginFinalCallback.h"

#include "acthex/ActionScheduler.h"
#include "acthex/PluginActionBase.h"

DLVHEX_NAMESPACE_BEGIN

ActionPluginFinalCallback::ActionPluginFinalCallback(ProgramCtx& ctx,
		CtxDataPtr ctxDataPtr) :
		programCtx(ctx), ctxDataPtr(ctxDataPtr), registryPtr(ctx.registry()) {
}

void ActionPluginFinalCallback::operator()() {
	std::cerr << "\nActionPluginFinalCallback called" << std::endl;

#warning I ve to call the BestModelSelection function of the specified ActionAtom
	ctxDataPtr->iteratorBestModel = ctxDataPtr->bestModelsContainer.begin();

	std::cerr << "\nBestModel selected:" << std::endl;
	(*ctxDataPtr->iteratorBestModel)->interpretation->print(std::cerr);
	std::cerr << std::endl;

	std::cerr << "\nCall the executionModeController" << std::endl;
	ActionScheduler* scheduler = new ActionScheduler(ctxDataPtr, registryPtr);
	std::multimap<int, Tuple> multimapOfExecution;
	scheduler->executionModeController(multimapOfExecution);

	std::cerr << "\nThe MultiMapOfExecution:" << std::endl;
	std::cerr << "Precedence\tTuple" << std::endl;
	std::multimap<int, Tuple>::iterator itMMOE;
	for (itMMOE = multimapOfExecution.begin();
			itMMOE != multimapOfExecution.end(); itMMOE++) {
		std::cerr << itMMOE->first << "\t\t";
		const Tuple& tempTuple = itMMOE->second;
		ActionPlugin::printTuple(tempTuple, registryPtr);
	}

#warning I ve to call the executionModeRewriter function of the specified ActionAtom
	std::cerr << "\nCall the executionModeRewriter" << std::endl;
	std::list < std::set<Tuple> > listOfExecution;
	scheduler->executionModeRewriter(multimapOfExecution, listOfExecution);

	std::cerr << "\nThe ListOfExecution:" << std::endl;
	std::list<std::set<Tuple> >::iterator itLOE;
	for (itLOE = listOfExecution.begin(); itLOE != listOfExecution.end();
			itLOE++) {
		std::set < Tuple > &tempSet = (*itLOE);
		for (std::set<Tuple>::iterator itLOEs = tempSet.begin();
				itLOEs != tempSet.end(); itLOEs++) {
			const Tuple& tempTuple = (*itLOEs);
			ActionPlugin::printTuple(tempTuple, registryPtr);
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

			if (*tempTuple.begin() == ctxDataPtr->id_continue
					&& ctxDataPtr->iterationType == DEFAULT) {
				ctxDataPtr->continueIteration = true;
				continue;
			} else if (*tempTuple.begin() == ctxDataPtr->id_stop
					&& ctxDataPtr->iterationType == FIXED) {
				ctxDataPtr->stopIteration = true;
				continue;
			}

			Tuple tupleForExecute;
			tupleForExecute.insert(tupleForExecute.begin(),
					tempTuple.begin() + 1, tempTuple.end());

			std::cerr << "tempTuple: ";
			ActionPlugin::printTuple(tempTuple, registryPtr);
			std::cerr << "tupleForExecute: ";
			ActionPlugin::printTuple(tupleForExecute, registryPtr);

			std::map<std::string, PluginActionBasePtr>::iterator it =
					ctxDataPtr->namePluginActionBaseMap.find(
							registryPtr->getTermStringByID(*tempTuple.begin()));

			if (it != ctxDataPtr->namePluginActionBaseMap.end())
				it->second->execute(programCtx,
						(*(ctxDataPtr->iteratorBestModel))->interpretation,
						tupleForExecute);
			else
				std::cerr << "For the action '"
						<< registryPtr->getTermStringByID(*tempTuple.begin())
						<< "' wasn't found a definition" << std::endl;

		}
	}

	std::cerr << "\nCheck Iteration" << std::endl;
	std::cerr << "IterationType:"
			<< (ctxDataPtr->iterationType == 0 ? "DEFAULT" : "FIXED")
			<< std::endl;
	if (ctxDataPtr->iterationType == DEFAULT && ctxDataPtr->continueIteration)
		programCtx.config.setOption("RepeatEvaluation", 1);
	else if (ctxDataPtr->iterationType == FIXED && ctxDataPtr->stopIteration)
		programCtx.config.setOption("RepeatEvaluation", 0);
	else if (ctxDataPtr->iterationType == FIXED) //FIXME only to try if it works
		programCtx.config.setOption("RepeatEvaluation", 1); //FIXME only to try if it works

	if (programCtx.config.getOption("RepeatEvaluation") > 0) {

		std::cerr << "\nClear data structures" << std::endl;
		ctxDataPtr->clearDataStructures();

		ctxDataPtr->continueIteration = false;
		ctxDataPtr->stopIteration = false;

		std::cerr << "\nReset cache" << std::endl;
		programCtx.resetCacheOfPlugins();

	}

}

DLVHEX_NAMESPACE_END
