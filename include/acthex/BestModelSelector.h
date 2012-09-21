/**
 * @file BestModelSelector.h
 * @author Stefano Germano
 *
 * @brief Base class to implement a Selector of BestMoldel
 */

#ifndef BEST_MODEL_SELECTOR_H_
#define BEST_MODEL_SELECTOR_H_

#include "acthex/ActionPluginCtxData.h"

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

	// a function that receives the Container of BestModels as a parameter
	// and must select the position of an Iterator (always received as a parameter)
	// indicating the desired BestModel
	virtual void getBestModel(
			ActionPluginCtxData::BestModelsContainer::const_iterator&,
			const ActionPluginCtxData::BestModelsContainer&) = 0;
private:
	// the name of this Selector, will be used to invoke it
	// must start with an alphabetic letter
	std::string name;
};
typedef boost::shared_ptr<BestModelSelector> BestModelSelectorPtr;

DLVHEX_NAMESPACE_END

#endif /* BEST_MODEL_SELECTOR_H_ */
