/**
 * @file ActionSchedulerCallback.h
 * @author Stefano Germano
 *
 * @brief ...
 */

#ifndef ACTION_SCHEDULER_H_
#define ACTION_SCHEDULER_H_

#include "acthex/ActionPlugin.h"

DLVHEX_NAMESPACE_BEGIN

class ActionScheduler {
public:
	ActionScheduler(const CtxDataPtr, const RegistryPtr registry);
	void executionModeController(std::multimap<int, Tuple>&);
#warning can be removed because it s implemented in DefaultExecutionModeRewriter
	void executionModeRewriter(const std::multimap<int, Tuple>&,
			std::list<std::set<Tuple> >&);
	bool checkIfTheListIsCorrect(const std::multimap<int, Tuple>&,
			const std::list<std::set<Tuple> >&);
private:
//	const ActionPlugin::CtxData& ctxData;
	const CtxDataPtr ctxDataPtr;
	const RegistryPtr registryPtr;
	bool isPresentInAllAnswerset(const Tuple&);
	bool isPresentInAllTheBestModelsAnswerset(const Tuple&);
	bool thisAnswerSetContainsThisAction(const AnswerSetPtr&, const Tuple&);
//    bool theMultiMapContainsThisActionWithThisPrecedence(const std::multimap<int, Tuple>&,
//        const Tuple&, int);
	bool checkIfThisSetsOfTupleContainsTheSameElements(const std::set<Tuple>&,
			const std::set<Tuple>&) const;
};

DLVHEX_NAMESPACE_END

#endif /* ACTION_SCHEDULER_H_ */
