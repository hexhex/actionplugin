/**
 * @file ActionSchedulerCallback.h
 * @author Stefano Germano
 *
 * @brief ...
 */

#ifndef ACTION_SCHEDULER_H_
#define ACTION_SCHEDULER_H_

#include "ActionPlugin.h"

DLVHEX_NAMESPACE_BEGIN

class ActionScheduler {
  public:
    ActionScheduler(const ActionPlugin::CtxData& ctxData, const RegistryPtr registry);
    void executionModeController(std::multimap<int, Tuple>&);
    void executionModeRewriter(const std::multimap<int, Tuple>&, std::list<std::set<Tuple> >&);
    bool checkIfTheListIsCorrect(const std::multimap<int, Tuple>&,
        const std::list<std::set<Tuple> >&);
  private:
    const ActionPlugin::CtxData& ctxData;
    const RegistryPtr registryPtr;
    bool isPresentInAllAnswerset(const Tuple&);
    bool isPresentInAllTheBestModelsAnswerset(const Tuple&);
    bool thisAnswerSetContainsThisAction(const AnswerSetPtr&, const Tuple&);
//    bool theMultiMapContainsThisActionWithThisPrecedence(const std::multimap<int, Tuple>&,
//        const Tuple&, int);
    bool checkIfThisSetsOfTupleContainsTheSameElements(const std::set<Tuple>&, const std::set<Tuple>&) const;
};

DLVHEX_NAMESPACE_END

#endif /* ACTION_SCHEDULER_H_ */
