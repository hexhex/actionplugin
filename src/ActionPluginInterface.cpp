/**
 * @file ActionPluginInterface.cpp
 * @author Stefano Germano
 *
 * @brief Interface to create Plugin of ActionPlugin
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H
#include "acthex/ActionPluginInterface.h"

#include "acthex/ActionPlugin.h"

DLVHEX_NAMESPACE_BEGIN

ActionPluginInterface::~ActionPluginInterface() {
}

// Used to activate the Plugin only if "--action-enable" option is selected
void ActionPluginInterface::processOptions(
		std::list<const char*>& pluginOptions, ProgramCtx& ctx) {
	DBGLOG(DBG, "processOptions of ActionPluginInterface");

	ActionPluginCtxData& ctxData = ctx.getPluginData<ActionPlugin>();

	typedef std::list<const char*>::iterator Iterator;
	Iterator it;

	it = pluginOptions.begin();
	while (it != pluginOptions.end()) {
		bool processed = false;
		const std::string str(*it);
		if (str == "--action-enable") {
			ctxData.enabled = true;
			processed = true;
		}

		if (processed) {
			// return value of erase: element after it, maybe end()
			DBGLOG(DBG,
					"ActionPluginInterface successfully processed option "
							<< str);
			it = pluginOptions.erase(it);
		} else {
			it++;
		}

	}

	if (ctxData.enabled)
		// Register the Action in the ActionPlugin
		ctxData.registerPlugin(this->create(ctx), ctx);

}

DLVHEX_NAMESPACE_END
