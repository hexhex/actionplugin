/**
 * @file ActionPluginModelCallback.cpp
 * @author Stefano Germano
 *
 * @brief Class that contains the functions used to Schedule the Actions
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H
#include "acthex/ActionScheduler.h"

DLVHEX_NAMESPACE_BEGIN

ActionScheduler::ActionScheduler(ActionPluginCtxData& ctxData,
		const RegistryPtr registryPtr) :
		ctxData(ctxData), registryPtr(registryPtr) {
}

// function that fill the multimap (passed as parameter)
// with Precedence attribute and Action Tuple
// checking the Action Option attribute
void ActionScheduler::executionModeController(
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

			std::cerr << "actionOption: "
					<< registryPtr->getTermStringByID(id_actionOption)
					<< std::endl;
			std::cerr << "Precedence: " << precedence << std::endl;

			if (id_actionOption == ctxData.id_brave)
				;
			else if (id_actionOption == ctxData.id_cautious)
				if (!isPresentInAllAnswerset(action_tuple)) {
					std::cerr
							<< "This action atom isn't present in all AnswerSet: ";
					ActionScheduler::printTuple(action_tuple, registryPtr);
					continue;
				} else if (id_actionOption == ctxData.id_preferred_cautious)
					if (!isPresentInAllTheBestModelsAnswerset(action_tuple)) {
						std::cerr
								<< "This action atom isn't present in all BestModels AnswerSet: ";
						ActionScheduler::printTuple(action_tuple, registryPtr);
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

bool ActionScheduler::isPresentInAllAnswerset(const Tuple& action_tuple) {

	if (!isPresentInAllTheBestModelsAnswerset(action_tuple))
		return false;

	ActionPluginCtxData::BestModelsContainer::const_iterator itBMC;
	for (itBMC = ctxData.notBestModelsContainer.begin();
			itBMC != ctxData.notBestModelsContainer.end(); itBMC++)
		if (!thisAnswerSetContainsThisAction(*itBMC, action_tuple))
			return false;

	return true;

}

bool ActionScheduler::isPresentInAllTheBestModelsAnswerset(
		const Tuple& action_tuple) {

	ActionPluginCtxData::BestModelsContainer::const_iterator itBMC;
	for (itBMC = ctxData.bestModelsContainer.begin();
			itBMC != ctxData.bestModelsContainer.end(); itBMC++)
		if (!(thisAnswerSetContainsThisAction(*itBMC, action_tuple)))
			return false;

	return true;

}

bool ActionScheduler::thisAnswerSetContainsThisAction(
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
bool ActionScheduler::checkIfTheListIsCorrect(
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

bool ActionScheduler::checkIfThisSetsOfTupleContainsTheSameElements(
		const std::set<Tuple>& set1, const std::set<Tuple>& set2) const {

	if (set1.size() != set2.size())
		return false;

	for (std::set<Tuple>::iterator it1 = set1.begin(); it1 != set1.end(); it1++)
		if (set2.find(*it1) == set2.end())
			return false;

	return true;

}

DLVHEX_NAMESPACE_END
