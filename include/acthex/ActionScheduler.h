/**
 * @file ActionSchedulerCallback.h
 * @author Stefano Germano
 *
 * @brief Class that contains the functions used to Schedule the Actions
 */

#ifndef ACTION_SCHEDULER_H_
#define ACTION_SCHEDULER_H_

#include "acthex/ActionPluginCtxData.h"

DLVHEX_NAMESPACE_BEGIN

class ActionScheduler {
public:
	ActionScheduler(ActionPluginCtxData&, const RegistryPtr registry);
	// function that fill the multimap (passed as parameter)
	// with Precedence attribute and Action Tuple
	// checking the Action Option attribute
	void executionModeController(std::multimap<int, Tuple>&);
	// function that check if the order of execution of Actions
	// in the List, based on their Precedence attribute, is correct
	// and if in the List there are all the Actions
	bool checkIfTheListIsCorrect(const std::multimap<int, Tuple>&,
			const std::list<std::set<Tuple> >&);

	// Utility function that prints a Tuple on standard error
	static void printTuple(const Tuple& tuple, const RegistryPtr registryPtr) {
		bool first = true;
		for (Tuple::const_iterator it = tuple.begin(); it != tuple.end();
				it++) {
			if (first)
				first = !first;
			else
				std::cerr << ", ";
			if (it->isConstantTerm() || it->isVariableTerm())
				std::cerr << registryPtr->getTermStringByID(*it);
			else
				std::cerr << it->address;
		}
		std::cerr << std::endl;
	}

private:
	ActionPluginCtxData& ctxData;
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
