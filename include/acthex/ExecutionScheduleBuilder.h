/**
 * @file ExecutionScheduleBuilder.h
 * @author Stefano Germano
 *
 * @brief Base class to implement a Scheduler for the Actions with the same Precedence attribute
 */

#ifndef EXECUTION_SCHEDULE_BUILDER_H_
#define EXECUTION_SCHEDULE_BUILDER_H_

#include "dlvhex2/PlatformDefinitions.h"
#include "dlvhex2/ID.h"
#include "dlvhex2/Interpretation.h"

#include <string>
#include <map>
#include <list>
using namespace std;

#include <boost/shared_ptr.hpp>

DLVHEX_NAMESPACE_BEGIN

class ExecutionScheduleBuilder {
public:
	ExecutionScheduleBuilder(std::string name) :
			name(name) {
	}

	virtual ~ExecutionScheduleBuilder();

	std::string getName() const {
		return name;
	}

	/**
	 * @brief receives the MultiMap of Execution as a parameter and must fill a List of Execution (always received as a parameter) indicating the desired order for the Actions with the same Precedence
	 */
	virtual void rewrite(const std::multimap<int, Tuple>&,
			std::list<std::set<Tuple> >&,
			const InterpretationConstPtr) = 0;
private:
	/**
	 * @brief the name of this Scheduler, will be used to invoke it; must start with an alphabetic letter
	 */
	std::string name;
};
typedef boost::shared_ptr<ExecutionScheduleBuilder> ExecutionScheduleBuilderPtr;

DLVHEX_NAMESPACE_END

#endif /* EXECUTION_SCHEDULE_BUILDER_H_ */
