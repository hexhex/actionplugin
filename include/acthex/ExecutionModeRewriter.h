/**
 * @file ExecutionModeRewriter.h
 * @author Stefano Germano
 *
 * @brief Base class to implement a Scheduler for the Actions with the same Precedence attribute
 */

#ifndef EXECUTION_MODE_REWRITER_H_
#define EXECUTION_MODE_REWRITER_H_

#include "acthex/ActionPlugin.h"

DLVHEX_NAMESPACE_BEGIN

class ExecutionModeRewriter {
public:
	ExecutionModeRewriter(std::string name) :
			name(name) {
	}

	virtual ~ExecutionModeRewriter();

	std::string getName() const {
		return name;
	}

	// a function that receives the MultiMap of Execution as a parameter
	// and must fill a List of Execution (always received as a parameter)
	// indicating the desired order for the Actions with the same Precedence
	virtual void rewrite(const std::multimap<int, Tuple>& multimapOfExecution,
			std::list<std::set<Tuple> >& listOfExecution) = 0;
private:
	// the name of this Scheduler, will be used to invoke it
	std::string name;
};
typedef boost::shared_ptr<ExecutionModeRewriter> ExecutionModeRewriterPtr;

DLVHEX_NAMESPACE_END

#endif /* EXECUTION_MODE_REWRITER_H_ */
