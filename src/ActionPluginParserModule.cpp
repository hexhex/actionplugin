/**
 * @file ActionPluginParserModule.cpp
 * @author Stefano Germano
 *
 * @brief Parser for the Actions
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H
#include "acthex/ActionPluginParserModule.h"

#include "acthex/ActionPlugin.h"

DLVHEX_NAMESPACE_BEGIN

ActionPluginParserModuleSemantics::ActionPluginParserModuleSemantics(
		ProgramCtx& ctx) :
		HexGrammarSemantics(ctx), ctxData(ctx.getPluginData<ActionPlugin>()) {
}

DLVHEX_NAMESPACE_END
