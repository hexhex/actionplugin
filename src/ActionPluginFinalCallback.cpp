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

ActionPluginFinalCallback::ActionPluginFinalCallback(
		ActionPlugin::CtxData& ctxData, const RegistryPtr registryPtr) :
		ctxData(ctxData), registryPtr(registryPtr) {
}

void ActionPluginFinalCallback::operator()() {
	std::cerr << "\nActionPluginFinalCallback called" << std::endl;

//  ActionPlugin::CtxData& ctxData = ctx.getPluginData<ActionPlugin>();

#warning I ve to call the BestModelSelection function of the specified ActionAtom
	//selectBestModel(const ActionPlugin::CtxData::BestModelsContainer& bestModelsContainer)
	//iteratorBestModel = myActionAtom.selectBestModel(bestModelsContainer);

	ctxData.iteratorBestModel = ctxData.bestModelsContainer.begin();

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
		ActionPlugin::printTuple(tempTuple, registryPtr);
	}

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

			Tuple tupleForExecute;
			tupleForExecute.insert(tupleForExecute.begin(),
					tempTuple.begin() + 1, tempTuple.end());

			std::cerr << "tempTuple: ";
			ActionPlugin::printTuple(tempTuple, registryPtr);
			std::cerr << "tupleForExecute: ";
			ActionPlugin::printTuple(tupleForExecute, registryPtr);

			std::map<std::string, PluginActionBasePtr>::iterator it =
					ctxData.namePluginActionBaseMap.find(
							registryPtr->getTermStringByID(*tempTuple.begin()));

			if (it != ctxData.namePluginActionBaseMap.end())
				it->second->execute(registryPtr, tupleForExecute);
			else
				std::cerr << "For the action '"
						<< registryPtr->getTermStringByID(*tempTuple.begin())
						<< "' wasn't found a definition" << std::endl;

		}
	}

}

DLVHEX_NAMESPACE_END
