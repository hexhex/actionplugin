/**
 * @file ActionPluginFinalCallback.h
 * @author Stefano Germano
 *
 * @brief ...
 */

#ifndef ACTION_PLUGIN_FINAL_CALLBACK_H_
#define ACTION_PLUGIN_FINAL_CALLBACK_H_

#include "ActionPlugin.h"

#include "dlvhex2/PluginInterface.h"

DLVHEX_NAMESPACE_BEGIN

class ActionPluginFinalCallback: public FinalCallback {
  public:
    ActionPluginFinalCallback(ActionPlugin::CtxData&, const RegistryPtr);
    virtual ~ActionPluginFinalCallback() {
    }
    virtual void operator()();
  protected:
    ActionPlugin::CtxData& ctxData;
    const RegistryPtr registryPtr;
};

DLVHEX_NAMESPACE_END

#endif /* ACTION_PLUGIN_FINAL_CALLBACK_H_ */
