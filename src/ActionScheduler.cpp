/**
 * @file ActionPluginModelCallback.cpp
 * @author Stefano Germano
 *
 * @brief ...
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H
#include "acthex/ActionScheduler.h"

#include "acthex/ActionPlugin.h"

DLVHEX_NAMESPACE_BEGIN

ActionScheduler::ActionScheduler(const ActionPlugin::CtxData& ctxData,
		const RegistryPtr registryPtr) :
		ctxData(ctxData), registryPtr(registryPtr) {
}

void ActionScheduler::executionModeController(
		std::multimap<int, Tuple>& multimapOfExecution) {

	const AnswerSetPtr& bestModel = (*ctxData.iteratorBestModel);

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

//    std::cerr << "action_tuple: ";
//    for (int i = 0; i < action_tuple.size(); i++)
//      std::cerr << registryPtr->getTermStringByID(action_tuple[i]) << ", ";
//    std::cerr << std::endl;

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
					ActionPlugin::printTuple(action_tuple, registryPtr);
					continue;
				} else if (id_actionOption == ctxData.id_preferred_cautious)
					if (!isPresentInAllTheBestModelsAnswerset(action_tuple)) {
						std::cerr
								<< "This action atom isn't present in all BestModels AnswerSet: ";
						ActionPlugin::printTuple(action_tuple, registryPtr);
						continue;
					} else
						throw PluginError(
								"Error in the selection of brave, cautious or preferred_cautious");

			multimapOfExecution.insert(
					std::pair<int, Tuple>(precedence, action_tuple));

		} else
			throw PluginError(
					"Precedence isn't Integer term or ??? haven't a valid value");

	}

}

bool ActionScheduler::isPresentInAllAnswerset(const Tuple& action_tuple) {

	if (!isPresentInAllTheBestModelsAnswerset(action_tuple))
		return false;

	ActionPlugin::CtxData::BestModelsContainer::const_iterator itBMC;
	for (itBMC = ctxData.notBestModelsContainer.begin();
			itBMC != ctxData.notBestModelsContainer.end(); itBMC++)
		if (!thisAnswerSetContainsThisAction(*itBMC, action_tuple))
			return false;

	return true;

}

bool ActionScheduler::isPresentInAllTheBestModelsAnswerset(
		const Tuple& action_tuple) {

	ActionPlugin::CtxData::BestModelsContainer::const_iterator itBMC;
	for (itBMC = ctxData.bestModelsContainer.begin();
			itBMC != ctxData.bestModelsContainer.end(); itBMC++)
		if (!(thisAnswerSetContainsThisAction(*itBMC, action_tuple)))
			return false;

	return true;

}

bool ActionScheduler::thisAnswerSetContainsThisAction(
		const AnswerSetPtr& answerSetPtr, const Tuple& action_tuple) {

	InterpretationPtr intr = InterpretationPtr(new Interpretation(registryPtr));
	intr->getStorage() |= ctxData.myAuxiliaryPredicateMask.mask()->getStorage();
	intr->getStorage() &= answerSetPtr->interpretation->getStorage();

	Interpretation::TrueBitIterator bit, bit_end;
	for (boost::tie(bit, bit_end) = intr->trueBits(); bit != bit_end; ++bit) {

		const Tuple & tuple = registryPtr->ogatoms.getByAddress(*bit).tuple;
//    std::cerr << "Tuple (size=" << action_tuple.size() << "): ";

		if (tuple.size() - 5 == action_tuple.size()) {

			bool different = false;

			for (int i = 0; i < action_tuple.size() && !different; i++) {
//        std::cerr << registryPtr->getTermStringByID(tuple[i + 1]) << " ";
				if (tuple[i + 1] != action_tuple[i]) {
//          std::cerr << "difference: " << registryPtr->getTermStringByID(tuple[i + 1]) << " != "
//              << registryPtr->getTermStringByID(action_tuple[i]) << std::endl;
					different = true;
				}
			}

			if (!different)
				return true;

		}

	}

//  std::cerr << "thisAnswerSetContainsThisAction false with this AnswerSet: ";
//  answerSetPtr->interpretation->print(std::cerr);
//  std::cerr << " and this action_tuple ";
//  Tuple::const_iterator itLOEsT = action_tuple.begin();
//  std::cerr << registryPtr->getTermStringByID(*itLOEsT);
//  std::cerr << " with this input list: ";
//  itLOEsT++;
//  for (; itLOEsT != action_tuple.end(); itLOEsT++)
//    std::cerr << registryPtr->getTermStringByID(*itLOEsT) << ", ";
//  std::cerr << std::endl;

	return false;

}

void ActionScheduler::executionModeRewriter(
		const std::multimap<int, Tuple>& multimapOfExecution,
		std::list<std::set<Tuple> >& listOfExecution) {

	if (multimapOfExecution.empty())
		return;

	std::multimap<int, Tuple>::const_iterator it = multimapOfExecution.begin();
	int lastPrecedence = it->first;
	std::set < Tuple > currentSet;
	for (; it != multimapOfExecution.end(); it++) {

		std::cerr << "lastPrecedence: " << lastPrecedence << std::endl;
		std::cerr << "in multimap: " << it->first << " "
				<< registryPtr->getTermStringByID(it->second[0]) << std::endl;

		if (it->first != lastPrecedence) {
			listOfExecution.push_back(currentSet);
			currentSet.clear();
			lastPrecedence = it->first;
		}
		currentSet.insert(it->second);

	}

	listOfExecution.push_back(currentSet);

}

bool ActionScheduler::checkIfTheListIsCorrect(
		const std::multimap<int, Tuple>& multimapOfExecution,
		const std::list<std::set<Tuple> >& listOfExecution) {

	if (multimapOfExecution.empty())
		if (listOfExecution.empty())
			return true;
		else
			return false;

	std::multimap<int, Tuple>::const_iterator itMOE =
			multimapOfExecution.begin();
	int latestPrecedenceValue = itMOE->first;
	// a vector that contains all the precedences once
	//std::vector<int> precedenceValues;
	std::list < std::set<Tuple> > listOfExecutionFromMultimap;
//  precedenceValues.push_back(itMOE->first);
	std::set < Tuple > currentSet;
//  itMOE++;
	for (; itMOE != multimapOfExecution.end(); itMOE++) {
//    if (itMOE->first != latestPrecedenceValue) {
//      precedenceValues.push_back(itMOE->first);
//      latestPrecedenceValue = itMOE->first;
//    }
		if (itMOE->first != latestPrecedenceValue) {
			listOfExecutionFromMultimap.push_back(currentSet);
			currentSet.clear();
			latestPrecedenceValue = itMOE->first;
		}
		currentSet.insert(itMOE->second);
	}
	listOfExecutionFromMultimap.push_back(currentSet);

//  std::list<std::set<Tuple> >::const_iterator itLOE;
//  int sizeOfAllSetsOfList = 0;
//  int latestIndexOfPrecedenceValues = 0;
//  for (itLOE = listOfExecution.begin(), ; itLOE != listOfExecution.end(); itLOE++) {
//    const std::set<Tuple> &tempSet = (*itLOE);
//    sizeOfAllSetsOfList += tempSet.size();
//
//    if (tempSet.size()
//        != multimapOfExecution.count(precedenceValues[latestIndexOfPrecedenceValues++]))
//      return false;
//
//    for (std::set<Tuple>::iterator itLOEs = tempSet.begin(); itLOEs != tempSet.end(); itLOEs++) {
//      const Tuple& tempTuple = (*itLOEs);
//      if (!(theMultiMapContainsThisActionWithThisPrecedence(multimapOfExecution, tempTuple,
//          precedenceValues[latestIndexOfPrecedenceValues - 1])))
//        return false;
//    }
//  }
//
//  if (multimapOfExecution.size() != sizeOfAllSetsOfList)
//    return false;

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

//bool ActionScheduler::theMultiMapContainsThisActionWithThisPrecedence(
//    const std::multimap<int, Tuple>& multimapOfExecution, const Tuple& tuple, int precedence) {
//
//  for (std::multimap<int, Tuple>::const_iterator itMOE = multimapOfExecution.begin();
//      itMOE != multimapOfExecution.end(); itMOE++)
//    if (itMOE->first == precedence && itMOE->second.size() == tuple.size()) {
//
//      const Tuple& tempTuple = itMOE->second;
//      bool different = false;
//      for (int i = 0; i < tuple.size() && !different; i++)
//        if (tempTuple[i] != tuple[i])
//          different = true;
//
//      if (!different)
//        return true;
//
//    }
//
//  return false;
//
//}

DLVHEX_NAMESPACE_END
