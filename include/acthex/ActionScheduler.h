/**
 * @file ActionSchedulerCallback.h
 * @author Stefano Germano
 *
 * @brief Class that contains the functions used to Schedule the Actions
 */

#ifndef ACTION_SCHEDULER_H_
#define ACTION_SCHEDULER_H_

#include "acthex/ActionPlugin.h"

DLVHEX_NAMESPACE_BEGIN

class ActionScheduler {
public:
	ActionScheduler(ActionPlugin::CtxData&, const RegistryPtr registry);
	// function that fill the multimap (passed as parameter)
	// with Precedence attribute and Action Tuple
	// checking the Action Option attribute
	void executionModeController(std::multimap<int, Tuple>&);
	// function that check if the order of execution of Actions
	// in the List, based on their Precedence attribute, is correct
	// and if in the List there are all the Actions
	bool checkIfTheListIsCorrect(const std::multimap<int, Tuple>&,
			const std::list<std::set<Tuple> >&);
private:
	ActionPlugin::CtxData& ctxData;
	const RegistryPtr registryPtr;
	// Utility functions
	bool isPresentInAllAnswerset(const Tuple&);
	bool isPresentInAllTheBestModelsAnswerset(const Tuple&);
	bool thisAnswerSetContainsThisAction(const AnswerSetPtr&, const Tuple&);
	bool checkIfThisSetsOfTupleContainsTheSameElements(const std::set<Tuple>&,
			const std::set<Tuple>&) const;
};

DLVHEX_NAMESPACE_END

#endif /* ACTION_SCHEDULER_H_ */
