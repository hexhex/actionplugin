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
#include "acthex/PluginActionBase.h"
#include "acthex/BestModelSelector.h"
#include "acthex/ExecutionScheduleBuilder.h"

DLVHEX_NAMESPACE_BEGIN

ActionPluginFinalCallback::ActionPluginFinalCallback(ProgramCtx& ctx) :
		programCtx(ctx), ctxData(ctx.getPluginData<ActionPlugin>()), registryPtr(
				ctx.registry()) {
}

void ActionPluginFinalCallback::operator()() {
	DBGLOG(DBG, "\nActionPluginFinalCallback called");

	if (ctxData.nameBestModelSelectorMap.count(
			ctxData.bestModelSelectorSelected) == 0)
		throw PluginError("The BestModelSelector chosen doesn't exist");

	ctxData.nameBestModelSelectorMap[ctxData.bestModelSelectorSelected]->getBestModel(
			ctxData.iteratorBestModel, ctxData.bestModelsContainer);

	std::stringstream ss;
	(*ctxData.iteratorBestModel)->interpretation->print(ss);
	DBGLOG(DBG, "\nBestModel selected: " << ss.str());

	DBGLOG(DBG, "\nCall the executionModeController");
	std::multimap<int, Tuple> multimapOfExecution;
	executionModeController(multimapOfExecution);

	DBGLOG(DBG, "\nThe MultiMapOfExecution:");
	DBGLOG(DBG, "Precedence\tTuple");
	std::multimap<int, Tuple>::iterator itMMOE;
	for (itMMOE = multimapOfExecution.begin();
			itMMOE != multimapOfExecution.end(); itMMOE++) {
		const Tuple& tempTuple = itMMOE->second;
		std::stringstream ss;
		printTuple(ss, tempTuple, registryPtr);
		DBGLOG(DBG, itMMOE->first << "\t\t" << ss.str());
	}

	if (ctxData.nameExecutionScheduleBuilderMap.count(
			ctxData.executionScheduleBuilderSelected) == 0)
		throw PluginError("The ExecutionScheduleBuilder chosen doesn't exist");

	DBGLOG(DBG, "\nCall the executionScheduleBuilder");
	std::list < std::set<Tuple> > listOfExecution;
	ctxData.nameExecutionScheduleBuilderMap[ctxData.executionScheduleBuilderSelected]->rewrite(
			multimapOfExecution, listOfExecution);

	DBGLOG(DBG, "\nThe ListOfExecution:");
	std::list<std::set<Tuple> >::iterator itLOE;
	for (itLOE = listOfExecution.begin(); itLOE != listOfExecution.end();
			itLOE++) {
		std::set < Tuple > &tempSet = (*itLOE);
		for (std::set<Tuple>::iterator itLOEs = tempSet.begin();
				itLOEs != tempSet.end(); itLOEs++) {
			const Tuple& tempTuple = (*itLOEs);
			std::stringstream ss;
			printTuple(ss, tempTuple, registryPtr);
			DBGLOG(DBG, ss.str());
		}
	}

	DBGLOG(DBG,
			"\nControl if the order of Set in the List corresponds to their precedence");
	if (checkIfTheListIsCorrect(multimapOfExecution, listOfExecution)) {
		DBGLOG(DBG, "The List is correct");
	} else {
		DBGLOG(DBG, "The List isn't correct");
		throw PluginError(
				"The order of Set in the ListOfExecution doens't correspond to their precedence");
	}

	DBGLOG(DBG, "\nExecute the actions in the right order");

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

			std::stringstream ss;
			printTuple(ss, tempTuple, registryPtr);
			DBGLOG(DBG, "tupleForExecute: " << ss.str());
			ss.str("");
			printTuple(ss, tupleForExecute, registryPtr);
			DBGLOG(DBG, "tempTuple: " << ss.str());

			std::map<std::string, PluginActionBasePtr>::iterator it =
					ctxData.namePluginActionBaseMap.find(
							registryPtr->getTermStringByID(*tempTuple.begin()));

			if (it != ctxData.namePluginActionBaseMap.end())
				it->second->execute(programCtx,
						(*(ctxData.iteratorBestModel))->interpretation,
						tupleForExecute);
			else
				DBGLOG(DBG,
						"For the action '"
								<< registryPtr->getTermStringByID(
										*tempTuple.begin())
								<< "' wasn't found a definition");

		}
	}

	DBGLOG(DBG, "\nCheck Iteration");
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

		DBGLOG(DBG, "\nClear data structures");
		ctxData.clearDataStructures();

		ctxData.continueIteration = false;
		ctxData.stopIteration = false;

		DBGLOG(DBG, "\nReset cache");
		programCtx.resetCacheOfPlugins();

	}

}

// function that fill the multimap (passed as parameter)
// with Precedence attribute and Action Tuple
// checking the Action Option attribute
void ActionPluginFinalCallback::executionModeController(
		std::multimap<int, Tuple>& multimapOfExecution) {

	const AnswerSetPtr& bestModel = (*(ctxData.iteratorBestModel));

	// used to have only the Action Atoms
	InterpretationPtr intr = InterpretationPtr(new Interpretation(registryPtr));
	intr->getStorage() |= ctxData.myAuxiliaryPredicateMask.mask()->getStorage();
	intr->getStorage() &= bestModel->interpretation->getStorage();

	Interpretation::TrueBitIterator bit, bit_end;
	for (boost::tie(bit, bit_end) = intr->trueBits(); bit != bit_end; ++bit) {

		const OrdinaryAtom& oatom = registryPtr->ogatoms.getByAddress(*bit);
		const Tuple & tuple = oatom.tuple;

		int precedence, precedence_position = tuple.size() - 3;
		dlvhex::ID id_actionOption = tuple[tuple.size() - 4];

		Tuple action_tuple;

		for (int i = 1; i < tuple.size() - 4; i++)
			action_tuple.push_back(tuple[i]);

		if (tuple[precedence_position].isIntegerTerm()
				&& id_actionOption.isConstantTerm()) {

			precedence = tuple[precedence_position].address;

			DBGLOG(DBG,
					"actionOption: "
							<< registryPtr->getTermStringByID(id_actionOption));
			DBGLOG(DBG, "Precedence: " << precedence);

			if (id_actionOption == ctxData.id_brave)
				;
			else if (id_actionOption == ctxData.id_cautious)
				if (!isPresentInAllAnswerset(action_tuple)) {
					std::stringstream ss;
					printTuple(ss, action_tuple, registryPtr);
					DBGLOG(DBG,
							"This action atom isn't present in all AnswerSet: "
									<< ss.str());
					continue;
				} else if (id_actionOption == ctxData.id_preferred_cautious)
					if (!isPresentInAllTheBestModelsAnswerset(action_tuple)) {
						std::stringstream ss;
						printTuple(ss, action_tuple, registryPtr);
						DBGLOG(DBG,
								"This action atom isn't present in all BestModels AnswerSet: "
										<< ss.str());
						continue;
					} else
						throw PluginError(
								"Error in the selection of brave, cautious or preferred_cautious");

			multimapOfExecution.insert(
					std::pair<int, Tuple>(precedence, action_tuple));

		} else
			throw PluginError(
					"Precedence isn't Integer term or actionOption haven't a valid value");

	}

}

bool ActionPluginFinalCallback::isPresentInAllAnswerset(
		const Tuple& action_tuple) {

	if (!isPresentInAllTheBestModelsAnswerset(action_tuple))
		return false;

	ActionPluginCtxData::BestModelsContainer::const_iterator itBMC;
	for (itBMC = ctxData.notBestModelsContainer.begin();
			itBMC != ctxData.notBestModelsContainer.end(); itBMC++)
		if (!thisAnswerSetContainsThisAction(*itBMC, action_tuple))
			return false;

	return true;

}

bool ActionPluginFinalCallback::isPresentInAllTheBestModelsAnswerset(
		const Tuple& action_tuple) {

	ActionPluginCtxData::BestModelsContainer::const_iterator itBMC;
	for (itBMC = ctxData.bestModelsContainer.begin();
			itBMC != ctxData.bestModelsContainer.end(); itBMC++)
		if (!(thisAnswerSetContainsThisAction(*itBMC, action_tuple)))
			return false;

	return true;

}

bool ActionPluginFinalCallback::thisAnswerSetContainsThisAction(
		const AnswerSetPtr& answerSetPtr, const Tuple& action_tuple) {

	// used to have only the Action Atoms
	InterpretationPtr intr = InterpretationPtr(new Interpretation(registryPtr));
	intr->getStorage() |= ctxData.myAuxiliaryPredicateMask.mask()->getStorage();
	intr->getStorage() &= answerSetPtr->interpretation->getStorage();

	Interpretation::TrueBitIterator bit, bit_end;
	for (boost::tie(bit, bit_end) = intr->trueBits(); bit != bit_end; ++bit) {

		const Tuple & tuple = registryPtr->ogatoms.getByAddress(*bit).tuple;

		if (tuple.size() - 5 == action_tuple.size()) {

			bool different = false;

			for (int i = 0; i < action_tuple.size() && !different; i++)
				if (tuple[i + 1] != action_tuple[i])
					different = true;

			if (!different)
				return true;

		}

	}

	return false;

}

// function that check if the order of execution of Actions
// in the List, based on their Precedence attribute, is correct
// and if in the List there are all the Actions
bool ActionPluginFinalCallback::checkIfTheListIsCorrect(
		const std::multimap<int, Tuple>& multimapOfExecution,
		const std::list<std::set<Tuple> >& listOfExecution) {

	if (multimapOfExecution.empty())
		if (listOfExecution.empty())
			return true;
		else
			return false;

	// used to create a List (listOfExecutionFromMultimap)
	// with Tuples ordered by their Precedence attribute
	std::multimap<int, Tuple>::const_iterator itMOE =
			multimapOfExecution.begin();
	int latestPrecedenceValue = itMOE->first;
	std::list < std::set<Tuple> > listOfExecutionFromMultimap;
	std::set < Tuple > currentSet;
	for (; itMOE != multimapOfExecution.end(); itMOE++) {
		if (itMOE->first != latestPrecedenceValue) {
			listOfExecutionFromMultimap.push_back(currentSet);
			currentSet.clear();
			latestPrecedenceValue = itMOE->first;
		}
		currentSet.insert(itMOE->second);
	}
	listOfExecutionFromMultimap.push_back(currentSet);

	if (listOfExecution.size() != listOfExecutionFromMultimap.size())
		return false;

	std::list<std::set<Tuple> >::const_iterator itLOE, itLOEFM;
	for (itLOE = listOfExecution.begin(), itLOEFM =
			listOfExecutionFromMultimap.begin();
			itLOE != listOfExecution.end(), itLOEFM
					!= listOfExecutionFromMultimap.end(); itLOE++, itLOEFM++)
		if (!checkIfThisSetsOfTupleContainsTheSameElements(*itLOEFM, *itLOE))
			return false;

	return true;

}

bool ActionPluginFinalCallback::checkIfThisSetsOfTupleContainsTheSameElements(
		const std::set<Tuple>& set1, const std::set<Tuple>& set2) const {

	if (set1.size() != set2.size())
		return false;

	for (std::set<Tuple>::iterator it1 = set1.begin(); it1 != set1.end(); it1++)
		if (set2.find(*it1) == set2.end())
			return false;

	return true;

}

// Utility function that prints a Tuple on the specified ostream
void ActionPluginFinalCallback::printTuple(std::ostream& output,
		const Tuple& tuple, const RegistryPtr registryPtr) {
	bool first = true;
	for (Tuple::const_iterator it = tuple.begin(); it != tuple.end(); it++) {
		if (first)
			first = !first;
		else
			output << ", ";
		if (it->isConstantTerm() || it->isVariableTerm())
			output << registryPtr->getTermStringByID(*it);
		else
			output << it->address;
	}
}

DLVHEX_NAMESPACE_END
