/**
 * @file ExecutionModeRewriter.h
 * @author Stefano Germano
 *
 * @brief ...
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
	virtual void rewrite(const std::multimap<int, Tuple>& multimapOfExecution,
			std::list<std::set<Tuple> >& listOfExecution) = 0;
private:
	std::string name;
};
typedef boost::shared_ptr<ExecutionModeRewriter> ExecutionModeRewriterPtr;

DLVHEX_NAMESPACE_END

#endif /* EXECUTION_MODE_REWRITER_H_ */
