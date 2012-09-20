/**
 * @file BuiltInDeclarationsParserModule.cpp
 * @author Stefano Germano
 *
 * @brief Parser for the Built in Declarations
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H
#include "acthex/BuiltInDeclarationsParserModule.h"

#include "acthex/ActionPlugin.h"

DLVHEX_NAMESPACE_BEGIN

BuiltInDeclarationsParserModuleSemantics::BuiltInDeclarationsParserModuleSemantics(
		ProgramCtx& ctx) :
		HexGrammarSemantics(ctx), ctxData(ctx.getPluginData<ActionPlugin>()) {
}

DLVHEX_NAMESPACE_END
