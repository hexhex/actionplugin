/**
 * @file ActionPluginFinalCallback.h
 * @author Stefano Germano
 *
 * @brief ...
 */

#ifndef ACTION_PLUGIN_FINAL_CALLBACK_H_
#define ACTION_PLUGIN_FINAL_CALLBACK_H_

#include "acthex/ActionPlugin.h"

#include "dlvhex2/PluginInterface.h"

DLVHEX_NAMESPACE_BEGIN

class ActionPluginFinalCallback: public FinalCallback {
public:
	ActionPluginFinalCallback(ProgramCtx&, ActionPlugin::CtxData&);
	virtual ~ActionPluginFinalCallback() {
	}
	virtual void operator()();
protected:
	ProgramCtx& programCtx;
	ActionPlugin::CtxData& ctxData;
	const RegistryPtr registryPtr;
};

DLVHEX_NAMESPACE_END

#endif /* ACTION_PLUGIN_FINAL_CALLBACK_H_ */
