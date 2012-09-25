/**
 * @file DefaultExecutionScheduleBuilder.h
 * @author Stefano Germano
 *
 * @brief A default implementation of ExecutionScheduleBuilder
 */

#ifndef DEFAULT_EXECUTION_SCHEDULE_BUILDER_H_
#define DEFAULT_EXECUTION_SCHEDULE_BUILDER_H_

#include "acthex/ExecutionScheduleBuilder.h"

DLVHEX_NAMESPACE_BEGIN

class DefaultExecutionScheduleBuilder: public ExecutionScheduleBuilder {
public:
	DefaultExecutionScheduleBuilder(std::string name) :
			ExecutionScheduleBuilder(name) {
	}
	virtual void rewrite(const std::multimap<int, Tuple>& multimapOfExecution,
			std::list<std::set<Tuple> >& listOfExecution,
			const InterpretationConstPtr interpretationConstPtr) {

		if (multimapOfExecution.empty())
			return;

		std::multimap<int, Tuple>::const_iterator it =
				multimapOfExecution.begin();
		int lastPrecedence = it->first;
		std::set < Tuple > currentSet;
		for (; it != multimapOfExecution.end(); it++) {

			if (it->first != lastPrecedence) {
				listOfExecution.push_back(currentSet);
				currentSet.clear();
				lastPrecedence = it->first;
			}
			currentSet.insert(it->second);

		}

		listOfExecution.push_back(currentSet);

	}
};

DLVHEX_NAMESPACE_END

#endif /* DEFAULT_EXECUTION_SCHEDULE_BUILDER_H_ */
