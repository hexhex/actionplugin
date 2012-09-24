/* dlvhex -- Answer-Set Programming with external interfaces.
 * Copyright (C) 2005, 2006, 2007 Roman Schindlauer
 * Copyright (C) 2006, 2007, 2008, 2009, 2010, 2011 Thomas Krennwallner
 * Copyright (C) 2009, 2010, 2011 Peter Schüller
 * 
 * This file is part of dlvhex.
 *
 * dlvhex is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * dlvhex is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with dlvhex; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA.
 */

/**
 * @file ActionPlugin.cpp
 * @author Stefano Germano
 *
 * @brief Plugin that provides an implementation of the ActHEX language.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H
//#define BOOST_SPIRIT_DEBUG
#include "acthex/ActionPlugin.h"

#include "acthex/ActionPluginModelCallback.h"
#include "acthex/ActionPluginFinalCallback.h"
#include "acthex/ActionParserModule.h"
#include "acthex/ActionPluginInterface.h"
#include "acthex/BuiltInDeclarationsParserModule.h"

#include <dlvhex2/PredicateMask.h>

#include <boost/date_time/posix_time/time_parsers.hpp>

DLVHEX_NAMESPACE_BEGIN

ActionPlugin::ActionPlugin() :
		PluginInterface() {
	setNameVersion("dlvhex-actionplugin", 2, 0, 0);
}

ActionPlugin::~ActionPlugin() {

}

// output help message for this plugin
void ActionPlugin::printUsage(std::ostream& o) const {
	//    123456789-123456789-123456789-123456789-123456789-123456789-123456789-123456789-
	o << "     --action-enable   Enable action plugin." << std::endl;
}

// accepted options: --action-enable
//					 --num-iterations
//					 --duration-iterations
//
// processes options for this plugin, and removes recognized options from pluginOptions
// (do not free the pointers, the const char* directly come from argv)
void ActionPlugin::processOptions(std::list<const char*>& pluginOptions,
		ProgramCtx& ctx) {

	ActionPlugin::CtxData& ctxData = ctx.getPluginData<ActionPlugin>();

	typedef std::list<const char*>::iterator Iterator;
	Iterator it;

	it = pluginOptions.begin();
	while (it != pluginOptions.end()) {
		bool processed = false;
		const std::string option(*it);
		if (option == "--action-enable") {
			ctxData.enabled = true;
			processed = true;
		} else if (option.find("--acthexNumberIterations=")
				!= std::string::npos) {
			const std::string string_of_number = option.substr(25);
			unsigned int number;
			try {
				number = boost::lexical_cast<unsigned int>(string_of_number);
			} catch (boost::bad_lexical_cast& e) {
				throw PluginError("Wrong value for --acthexNumberIterations");
			}
			//qi::parse(string_of_number.begin(), string_of_number.end(), number);
			ctxData.addNumberIterations(number, ctx, false);
			processed = true;
		} else if (option.find("--acthexDurationIterations=")
				!= std::string::npos) {
			const std::string string_of_duration = option.substr(27);
#warning Duration specified in seconds
			unsigned int duration;
			try {
				duration = boost::lexical_cast<unsigned int>(
						string_of_duration);
			} catch (boost::bad_lexical_cast& e) {
				throw PluginError("Wrong value for --acthexDurationIterations");
			}
			ctxData.addDurationIterations(duration, false);
			//ctxdata.addDurationIterations(string_of_duration);
			processed = true;
		}

		if (processed) {
			// return value of erase: element after it, maybe end()
			DBGLOG(DBG,
					"ActionPlugin successfully processed option " << option);
			it = pluginOptions.erase(it);
		} else {
			it++;
		}

	}

	if (ctxData.enabled) {

		RegistryPtr reg = ctx.registry();

		ctxData.id_brave = reg->storeConstantTerm("b");
		ctxData.id_cautious = reg->storeConstantTerm("c");
		ctxData.id_preferred_cautious = reg->storeConstantTerm("c_p");

		// so this kind of actions will be executed first (indeed will be put ​​in the first set)
		ctxData.id_default_precedence = ID::termFromInteger(0);
		// because the action atom without weight and level don't have to influence the selection of BestModels
		ctxData.id_default_weight_without_level = ID::termFromInteger(0);
		ctxData.id_default_level_without_weight = ID::termFromInteger(0);
		// so the user can avoid specifying one of them if he want that it value is set at 1
		ctxData.id_default_weight_with_level = ID::termFromInteger(1);
		ctxData.id_default_level_with_weight = ID::termFromInteger(1);

		ctxData.createAndInsertContinueAndStopActions(reg);
		ctxData.insertDefaultBestModelSelectorAndDefaultExecutionScheduleBuilder();

	}

}

// create parser modules that extend and the basic hex grammar
// this parser also stores the query information into the plugin
std::vector<HexParserModulePtr> ActionPlugin::createParserModules(
		ProgramCtx & ctx) {
	DBGLOG(DBG, "ActionPlugin::createParserModules()");

	std::vector < HexParserModulePtr > ret;
	ActionPlugin::CtxData& ctxdata = ctx.getPluginData<ActionPlugin>();

	if (ctxdata.enabled) {
		ret.push_back(
				HexParserModulePtr(
						new ActionParserModule<HexParserModule::HEADATOM>(
								ctx)));
		ret.push_back(
				HexParserModulePtr(
						new BuiltInDeclarationsParserModule<
								HexParserModule::TOPLEVEL>(ctx)));
	}

	return ret;
}

void ActionPlugin::setupProgramCtx(ProgramCtx& ctx) {

	ActionPlugin::CtxData& ctxData = ctx.getPluginData<ActionPlugin>();
	if (!ctxData.enabled)
		return;

	RegistryPtr reg = ctx.registry();

	// init predicate mask
	ctxData.myAuxiliaryPredicateMask.setRegistry(reg);

	ModelCallbackPtr mcb(new ActionPluginModelCallback(ctxData, reg));
	ctx.modelCallbacks.clear();
	ctx.modelCallbacks.push_back(mcb);

	FinalCallbackPtr finalCallbackPtr(new ActionPluginFinalCallback(ctx));
	ctx.finalCallbacks.push_back(finalCallbackPtr);

}

//
// now instantiate the plugin
//
ActionPlugin theActionPlugin;

DLVHEX_NAMESPACE_END

//
// let it be loaded by dlvhex!
//
IMPLEMENT_PLUGINABIVERSIONFUNCTION

// return plain C type s.t. all compilers and linkers will like this code
extern "C" void * PLUGINIMPORTFUNCTION() {
return reinterpret_cast<void*>(& DLVHEX_NAMESPACE theActionPlugin);
}
