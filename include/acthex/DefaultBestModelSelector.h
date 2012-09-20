/**
 * @file DefaultBestModelSelector.h
 * @author Stefano Germano
 *
 * @brief A default implementation of BestModelSelector
 */

#ifndef DEFAULT_BEST_MODEL_SELECTOR_H_
#define DEFAULT_BEST_MODEL_SELECTOR_H_

#include "acthex/BestModelSelector.h"

DLVHEX_NAMESPACE_BEGIN

class DefaultBestModelSelector: public BestModelSelector {
public:
	DefaultBestModelSelector(std::string name) :
			BestModelSelector(name) {
	}
	virtual void getBestModel(
			ActionPluginCtxData::BestModelsContainer::const_iterator& iteratorBestModel,
			const ActionPluginCtxData::BestModelsContainer& bestModelsContainer) {
		iteratorBestModel = bestModelsContainer.begin();
	}
};

DLVHEX_NAMESPACE_END

#endif /* DEFAULT_BEST_MODEL_SELECTOR_H_ */
