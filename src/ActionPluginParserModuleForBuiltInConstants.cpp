/**
 * @file ActionPluginParserModuleForBuiltInConstants.cpp
 * @author Stefano Germano
 *
 * @brief Parser for the Built in Constants
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H
#include "acthex/ActionPluginParserModuleForBuiltInConstants.h"

#include "acthex/ActionPlugin.h"

DLVHEX_NAMESPACE_BEGIN

ActionPluginParserModuleSemanticsForBuiltInConstants::ActionPluginParserModuleSemanticsForBuiltInConstants(
		ProgramCtx& ctx) :
		HexGrammarSemantics(ctx), ctxData(ctx.getPluginData<ActionPlugin>()) {
}

DLVHEX_NAMESPACE_END
