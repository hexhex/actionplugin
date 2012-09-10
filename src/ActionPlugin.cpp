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
 * @brief Plugin ...
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H
//#define BOOST_SPIRIT_DEBUG
#include "acthex/ActionPlugin.h"

#include "acthex/ActionPluginModelCallback.h"
#include "acthex/ActionPluginFinalCallback.h"
#include "acthex/ActionPluginParserModule.h"
#include "acthex/ActionPluginInterface.h"

#include "dlvhex2/PredicateMask.h"
DLVHEX_NAMESPACE_BEGIN

ActionPlugin::CtxData::CtxData() :
		enabled(false), idActionMap(), levelsAndWeightsBestModels(), bestModelsContainer(), notBestModelsContainer(), iteratorBestModel(), namePluginActionBaseMap(), iterationType(
				DEFAULT), continueIteration(false), stopIteration(false) {
}

ActionPlugin::CtxData::~CtxData() {
	idActionMap.clear();
	levelsAndWeightsBestModels.clear();
	bestModelsContainer.clear();
	notBestModelsContainer.clear();
	iteratorBestModel = bestModelsContainer.end();
	namePluginActionBaseMap.clear();
}

void ActionPlugin::CtxData::addAction(const ID & id,
		const ActionPtr actionPtr) {

	idActionMap.insert(std::pair<ID, ActionPtr>(id, actionPtr));

	myAuxiliaryPredicateMask.addPredicate(actionPtr->getAuxId());

}

void ActionPlugin::CtxData::registerPlugin(
		ActionPluginInterfacePtr actionPluginInterfacePtr, ProgramCtx& ctx) {

	std::cerr << "registerPlugin called" << std::endl;

	std::vector<PluginActionBasePtr> pluginActionBasePtrVector =
			actionPluginInterfacePtr->createActions(ctx);

	RegistryPtr reg = ctx.registry();

	for (std::vector<PluginActionBasePtr>::iterator it =
			pluginActionBasePtrVector.begin();
			it != pluginActionBasePtrVector.end(); it++) {

		const ID id = reg->storeConstantTerm((*it)->getPredicate());

		namePluginActionBaseMap.insert(
				std::pair<std::string, PluginActionBasePtr>(
						(*it)->getPredicate(), (*it)));

		std::cerr << "Inserted: " << (*it)->getPredicate() << std::endl;

		ID aux_id = reg->getAuxiliaryConstantSymbol('a', id);

		RawPrinter printer(std::cerr, reg);

		std::cerr << "Id: ";
		printer.print(id);
		std::cerr << "\t";
		printer.print(aux_id);
		std::cerr << std::endl;

		ActionPtr actionPtr(new Action(reg->getTermStringByID(id), aux_id));
		this->addAction(id, actionPtr);

	}

}

void ActionPlugin::CtxData::clearDataStructures() {
	levelsAndWeightsBestModels.clear();
	bestModelsContainer.clear();
	notBestModelsContainer.clear();
	iteratorBestModel = bestModelsContainer.end();
}

void ActionPlugin::CtxData::createAndInsertContinueAndStopActions(
		RegistryPtr reg) {

	id_continue = reg->storeConstantTerm("continueIteration");
	id_stop = reg->storeConstantTerm("stopIteration");

	RawPrinter printer(std::cerr, reg);

	ID aux_id_continue = reg->getAuxiliaryConstantSymbol('a', id_continue);
	ActionPtr actionPtrContinue(
			new Action(reg->getTermStringByID(id_continue), aux_id_continue));
	this->addAction(id_continue, actionPtrContinue);

	std::cerr << "id_continue : ";
	printer.print(id_continue);
	std::cerr << ",\t aux_id_continue: ";
	printer.print(aux_id_continue);
	std::cerr << std::endl;

	ID aux_id_stop = reg->getAuxiliaryConstantSymbol('a', id_stop);
	ActionPtr actionPtrStop(
			new Action(reg->getTermStringByID(id_stop), aux_id_stop));
	this->addAction(id_stop, actionPtrStop);

	std::cerr << "id_stop : ";
	printer.print(id_stop);
	std::cerr << ",\t aux_id_stop:";
	printer.print(aux_id_stop);
	std::cerr << std::endl;

}

ActionPlugin::ActionPlugin() :
		PluginInterface() {
#warning without it there isn t Segmentation Fault
//	setNameVersion("dlvhex-actionplugin", 2, 0, 0);
}

ActionPlugin::~ActionPlugin() {

}

// output help message for this plugin
void ActionPlugin::printUsage(std::ostream& o) const {
	//    123456789-123456789-123456789-123456789-123456789-123456789-123456789-123456789-
	o << "     --action-enable   Enable action plugin." << std::endl;
}

// accepted options: --action-enable
//
// processes options for this plugin, and removes recognized options from pluginOptions
// (do not free the pointers, the const char* directly come from argv)
void ActionPlugin::processOptions(std::list<const char*>& pluginOptions,
		ProgramCtx& ctx) {
	ActionPlugin::CtxData& ctxdata = ctx.getPluginData<ActionPlugin>();

	typedef std::list<const char*>::iterator Iterator;
	Iterator it;

	it = pluginOptions.begin();
	while (it != pluginOptions.end()) {
		bool processed = false;
		const std::string option(*it);
		if (option == "--action-enable") {
			ctxdata.enabled = true;
			processed = true;
		} else if (option == "--iterate-infinite") { //FIXME only to try if it works
			processed = true;
			ctxdata.iterationType = FIXED;
		} else if (option.find("--num-iterations=") != std::string::npos) {
			processed = true;
			ctxdata.iterationType = FIXED;
			std::string string_of_number = option.substr(17);
			unsigned int number;
			qi::parse(string_of_number.begin(), string_of_number.end(), number);
			ctx.config.setOption("RepeatEvaluation", number);
		}

		if (processed) {
			// return value of erase: element after it, maybe end()
			DBGLOG(DBG, "ActionPlugin successfully processed option " << option);
			it = pluginOptions.erase(it);
		} else {
			it++;
		}

	}

	if (ctxdata.enabled) {

		RegistryPtr reg = ctx.registry();

//    ctxdata.id_in_the_registry = reg->getAuxiliaryConstantSymbol('a',
//        dlvhex::ID(ID::MAINKIND_TERM | ID::SUBKIND_TERM_CONSTANT, 0));

		ctxdata.id_brave = reg->storeConstantTerm("b");
		ctxdata.id_cautious = reg->storeConstantTerm("c");
		ctxdata.id_preferred_cautious = reg->storeConstantTerm("c_p");

		// so this kind of actions will be executed first (indeed will be put ​​in the first set)
		ctxdata.id_default_precedence = ID::termFromInteger(0);
		// because the action atom without weight and level don't have to influence the selection of BestModels
		ctxdata.id_default_weight_without_level = ID::termFromInteger(0);
		ctxdata.id_default_level_without_weight = ID::termFromInteger(0);
		// so the user can avoid specifying one of them if he want that it value is set at 1
		ctxdata.id_default_weight_with_level = ID::termFromInteger(1);
		ctxdata.id_default_level_with_weight = ID::termFromInteger(1);

		std::cerr << "IterationType:"
				<< (ctxdata.iterationType == 0 ? "DEFAULT" : "FIXED")
				<< std::endl;

		ctxdata.createAndInsertContinueAndStopActions(reg);

	}

}

// create parser modules that extend and the basic hex grammar
// this parser also stores the query information into the plugin
std::vector<HexParserModulePtr> ActionPlugin::createParserModules(
		ProgramCtx & ctx) {
	DBGLOG(DBG, "ActionPlugin::createParserModules()");

	std::vector < HexParserModulePtr > ret;
	ActionPlugin::CtxData& ctxdata = ctx.getPluginData<ActionPlugin>();

	if (ctxdata.enabled)
		ret.push_back(
				HexParserModulePtr(
						new ActionPluginParserModule<HexParserModule::HEADATOM>(
								ctx)));

	return ret;
}

#warning is used only to create a CtxData that isn t destroyed
void deallocatorFunc(ActionPlugin::CtxData * ctxData) {
}

void ActionPlugin::setupProgramCtx(ProgramCtx& ctx) {

	ActionPlugin::CtxData& ctxdata = ctx.getPluginData<ActionPlugin>();
	if (!ctxdata.enabled)
		return;

	RegistryPtr reg = ctx.registry();

	// init predicate mask
	ctxdata.myAuxiliaryPredicateMask.setRegistry(reg);

	CtxDataPtr ctxDataPtr(&ctxdata, deallocatorFunc);

	ModelCallbackPtr mcb(new ActionPluginModelCallback(ctxDataPtr, reg));
	ctx.modelCallbacks.clear();
	ctx.modelCallbacks.push_back(mcb);

	FinalCallbackPtr finalCallbackPtr(
			new ActionPluginFinalCallback(ctx, ctxDataPtr));
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

//// this would be the code to use this plugin as a "real" plugin in a .so file
//// but we directly use it in dlvhex.cpp
//#if 0
//ActionPlugin theActionPlugin;
//
//// return plain C type s.t. all compilers and linkers will like this code
//extern "C"
//void * PLUGINIMPORTFUNCTION()
//{
//	return reinterpret_cast<void*>(& DLVHEX_NAMESPACE theActionPlugin);
//}
//
//#endif
///* vim: set noet sw=2 ts=2 tw=80: */
//
//// Local Variables:
//// mode: C++
//// End:
