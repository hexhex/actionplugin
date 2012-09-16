/**
 * @file ActionPluginModelCallback.h
 * @author Stefano Germano
 *
 * @brief A custom implementation of ModelCallback;
 * implement the Filter for Weights and Levels
 */

#ifndef ACTION_PLUGIN_MODEL_CALLBACK_H_
#define ACTION_PLUGIN_MODEL_CALLBACK_H_

#include "acthex/ActionPlugin.h"

#include "dlvhex2/PluginInterface.h"

DLVHEX_NAMESPACE_BEGIN

class ActionPluginModelCallback: public ModelCallback {
public:
	ActionPluginModelCallback(ActionPlugin::CtxData&, const RegistryPtr);
	virtual ~ActionPluginModelCallback();
	// This function will be called for each AnswerSet
	virtual bool operator()(AnswerSetPtr);
protected:
	int isABestModel(ActionPlugin::CtxData::LevelsAndWeights&,
			ActionPlugin::CtxData::LevelsAndWeights&);
	ActionPlugin::CtxData& ctxData;
	const RegistryPtr registryPtr;
};

DLVHEX_NAMESPACE_END

#endif /* ACTION_PLUGIN_MODEL_CALLBACK_H_ */
