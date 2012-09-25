/**
 * @file ActionPluginModelCallback.h
 * @author Stefano Germano
 *
 * @brief A custom implementation of ModelCallback;
 * implement the Filter for Weights and Levels
 */

#ifndef ACTION_PLUGIN_MODEL_CALLBACK_H_
#define ACTION_PLUGIN_MODEL_CALLBACK_H_

#include "acthex/ActionPluginCtxData.h"

#include "dlvhex2/PluginInterface.h"

DLVHEX_NAMESPACE_BEGIN

class ActionPluginModelCallback: public ModelCallback {
public:
	ActionPluginModelCallback(ActionPluginCtxData&, const RegistryPtr);
	virtual ~ActionPluginModelCallback();
	/**
	 * @brief Filter for Weights and Levels
	 *
	 * will be called for each AnswerSet
	 */
	virtual bool operator()(AnswerSetPtr);
protected:
	/**
	 * @brief If the AnswerSet with the LevelsAndWeights, passed as the second parameter, is a BestModel
	 *
	 * @return 	0 if it's a BestModel like the AnswerSets in bestModelsContainer
	 * 			-1 if it isn't a BestModel
	 * 			1 if it's a BestModel and it's better than the AnswerSets in bestModelsContainer
	 */
	int isABestModel(ActionPluginCtxData::LevelsAndWeights&,
			ActionPluginCtxData::LevelsAndWeights&);
	ActionPluginCtxData& ctxData;
	const RegistryPtr registryPtr;
};

DLVHEX_NAMESPACE_END

#endif /* ACTION_PLUGIN_MODEL_CALLBACK_H_ */
