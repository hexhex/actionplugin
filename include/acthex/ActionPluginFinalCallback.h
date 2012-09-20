/**
 * @file ActionPluginFinalCallback.h
 * @author Stefano Germano
 *
 * @brief A custom implementation of FinalCallback;
 * will be used for:
 *  - select the BestModel
 *  - build the Execution Scheduler
 *  - execute the Actions in the Execution Scheduler
 */

#ifndef ACTION_PLUGIN_FINAL_CALLBACK_H_
#define ACTION_PLUGIN_FINAL_CALLBACK_H_

#include "acthex/ActionPluginCtxData.h"

#include "dlvhex2/PluginInterface.h"

DLVHEX_NAMESPACE_BEGIN

class ActionPluginFinalCallback: public FinalCallback {
public:
	ActionPluginFinalCallback(ProgramCtx&);
	virtual ~ActionPluginFinalCallback() {
	}
	virtual void operator()();
protected:
	// function that fill the multimap (passed as parameter)
	// with Precedence attribute and Action Tuple
	// checking the Action Option attribute
	void executionModeController(std::multimap<int, Tuple>&);
	// function that check if the order of execution of Actions
	// in the List, based on their Precedence attribute, is correct
	// and if in the List there are all the Actions
	bool checkIfTheListIsCorrect(const std::multimap<int, Tuple>&,
			const std::list<std::set<Tuple> >&);
	// Utility functions
	bool isPresentInAllAnswerset(const Tuple&);
	bool isPresentInAllTheBestModelsAnswerset(const Tuple&);
	bool thisAnswerSetContainsThisAction(const AnswerSetPtr&, const Tuple&);
	bool checkIfThisSetsOfTupleContainsTheSameElements(const std::set<Tuple>&,
			const std::set<Tuple>&) const;
	// Utility function that prints a Tuple on standard error
	void printTuple(const Tuple& tuple, const RegistryPtr registryPtr);

	ProgramCtx& programCtx;
	ActionPluginCtxData& ctxData;
	const RegistryPtr registryPtr;
};

DLVHEX_NAMESPACE_END

#endif /* ACTION_PLUGIN_FINAL_CALLBACK_H_ */
