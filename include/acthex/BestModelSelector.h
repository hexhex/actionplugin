/**
 * @file BestModelSelector.h
 * @author Stefano Germano
 *
 * @brief ...
 */

#ifndef BEST_MODEL_SELECTOR_H_
#define BEST_MODEL_SELECTOR_H_

#include "acthex/ActionPlugin.h"

DLVHEX_NAMESPACE_BEGIN

class BestModelSelector {
public:
	BestModelSelector(std::string name) :
			name(name) {
	}
	virtual ~BestModelSelector();
	std::string getName() const {
		return name;
	}
	virtual void getBestModel(
			ActionPlugin::CtxData::BestModelsContainer::const_iterator&,
			const ActionPlugin::CtxData::BestModelsContainer&) = 0;
private:
	std::string name;
};
typedef boost::shared_ptr<BestModelSelector> BestModelSelectorPtr;

DLVHEX_NAMESPACE_END

#endif /* BEST_MODEL_SELECTOR_H_ */
