/**
 * @file ActionPluginModelCallback.h
 * @author Stefano Germano
 *
 * @brief ...
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
	virtual bool operator()(AnswerSetPtr);
protected:
	int isABestModel(ActionPlugin::CtxData&,
			ActionPlugin::CtxData::LevelsAndWeights&);
	//ProgramCtx& ctx;
	ActionPlugin::CtxData& ctxData;
	const RegistryPtr registryPtr;
};

DLVHEX_NAMESPACE_END

#endif /* ACTION_PLUGIN_MODEL_CALLBACK_H_ */
