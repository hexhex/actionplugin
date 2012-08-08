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

#include "ActionPlugin.h"
#include "ActionPluginModelCallback.h"
#include "ActionPluginFinalCallback.h"
#include "ActionPluginParserModule.h"

#include "dlvhex2/Registry.h"
#include "dlvhex2/PredicateMask.h"
//#include "dlvhex2/PlatformDefinitions.h"
//#include "dlvhex2/Printer.h"
//#include "dlvhex2/Printhelpers.h"
//#include "dlvhex2/Logger.h"
//#include "dlvhex2/HexParser.h"
//#include "dlvhex2/HexParserModule.h"
//#include "dlvhex2/HexGrammar.h"

#include "ActionPluginInterface.h"

DLVHEX_NAMESPACE_BEGIN

//namespace spirit = boost::spirit;
//namespace qi = boost::spirit::qi;

ActionPlugin::CtxData::CtxData() :
    enabled(false), idActionMap(), levelsAndWeightsBestModels(), bestModelsContainer(), notBestModelsContainer(), iteratorBestModel()
//, action_atom_name("aux_action_atom")
//, negPredicateArities()
{
}

void ActionPlugin::CtxData::addAction(const ID & id, const Action & action) {

  idActionMap.insert(std::pair<ID, Action>(id, action));

  myAuxiliaryPredicateMask.addPredicate(action.getAuxId());

}

void ActionPlugin::CtxData::registerPlugin(
    boost::shared_ptr<ActionPluginInterface> actionPluginInterfacePtr, ProgramCtx& ctx) {

  std::cerr << "\registerPlugin called" << std::endl;

  std::vector<PluginActionBasePtr> pluginActionBasePtrVector =
      actionPluginInterfacePtr->createActions(ctx);

  for (std::vector<PluginActionBasePtr>::iterator it = pluginActionBasePtrVector.begin();
      it != pluginActionBasePtrVector.end(); it++) {
    namePluginActionBaseMap.insert(
        std::pair<std::string, PluginActionBasePtr>((*it)->getPredicate(), (*it)));

    std::cerr << "Inserted: " << (*it)->getPredicate() << std::endl;
  }

}

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
//
// processes options for this plugin, and removes recognized options from pluginOptions
// (do not free the pointers, the const char* directly come from argv)
void ActionPlugin::processOptions(std::list<const char*>& pluginOptions, ProgramCtx& ctx) {
  ActionPlugin::CtxData& ctxdata = ctx.getPluginData<ActionPlugin>();

  typedef std::list<const char*>::iterator Iterator;
  Iterator it;
#warning create (or reuse, maybe from potassco?) cmdline option processing facility
  it = pluginOptions.begin();
  while (it != pluginOptions.end()) {
    bool processed = false;
    const std::string str(*it);
    if (str == "--action-enable") {
      ctxdata.enabled = true;
      processed = true;
    }

    if (processed) {
      // return value of erase: element after it, maybe end()
      DBGLOG(DBG, "ActionPlugin successfully processed option " << str);
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

  }

}

// create parser modules that extend and the basic hex grammar
// this parser also stores the query information into the plugin
std::vector<HexParserModulePtr> ActionPlugin::createParserModules(ProgramCtx & ctx) {
  DBGLOG(DBG, "ActionPlugin::createParserModules()");

  std::vector < HexParserModulePtr > ret;
  ActionPlugin::CtxData& ctxdata = ctx.getPluginData<ActionPlugin>();

  if (ctxdata.enabled)
    ret.push_back(HexParserModulePtr(new ActionPluginParserModule<HexParserModule::HEADATOM>(ctx)));

  return ret;
}

namespace {

  typedef ActionPlugin::CtxData CtxData;

  class ActionPluginConstraintAdder: public PluginRewriter {
    public:
      ActionPluginConstraintAdder() {
      }
      virtual ~ActionPluginConstraintAdder() {
      }

      virtual void rewrite(ProgramCtx& ctx);
  };

  void ActionPluginConstraintAdder::rewrite(ProgramCtx& ctx) {
//	typedef ActionPlugin::CtxData::PredicateArityMap PredicateArityMap;

//	DBGLOG_SCOPE(DBG,"neg_rewr",false);
//	DBGLOG(DBG,"= ActionPluginConstraintAdder::rewrite");
//
    ActionPlugin::CtxData& ctxdata = ctx.getPluginData<ActionPlugin>();
    assert(ctxdata.enabled && "this rewriter should only be used "
        "if the plugin is enabled");

    RegistryPtr reg = ctx.registry();
    assert(reg);
//	PredicateArityMap::const_iterator it;
//	for(it = ctxdata.negPredicateArities.begin();
//			it != ctxdata.negPredicateArities.end(); ++it)
//	{
//		// for predicate foo of arity k create constraint
//		// :- foo(X1,X2,...,Xk), foo_neg_aux(X1,X2,...,Xk).
//
//		// create atoms
//		const ID idpred = it->first;
//		const unsigned arity = it->second;
//		DBGLOG(DBG,"processing predicate '" <<
//				printToString<RawPrinter>(idpred, reg) << "'/" << idpred <<
//				" with arity " << arity);
//
//		const ID idnegpred = reg->getAuxiliaryConstantSymbol('s', idpred);
//		ID idatom;
//		ID idnegatom;
//		if( arity == 0 )
//		{
//			// ground atoms
//			OrdinaryAtom predAtom(
//					ID::MAINKIND_ATOM |
//					ID::SUBKIND_ATOM_ORDINARYG);
//			predAtom.tuple.push_back(idpred);
//			OrdinaryAtom negpredAtom(
//					ID::MAINKIND_ATOM |
//					ID::SUBKIND_ATOM_ORDINARYG |
//					ID::PROPERTY_AUX);
//			negpredAtom.tuple.push_back(idnegpred);
//			idatom = reg->storeOrdinaryGAtom(predAtom);
//			idnegatom = reg->storeOrdinaryGAtom(negpredAtom);
//		}
//		else
//		{
//			// nonground atoms
//			OrdinaryAtom predAtom(
//					ID::MAINKIND_ATOM |
//					ID::SUBKIND_ATOM_ORDINARYN);
//			predAtom.tuple.push_back(idpred);
//			OrdinaryAtom negpredAtom(
//					ID::MAINKIND_ATOM |
//					ID::SUBKIND_ATOM_ORDINARYN |
//					ID::PROPERTY_AUX);
//			negpredAtom.tuple.push_back(idnegpred);
//
//			// add variables
//			for(unsigned i = 0; i < arity; ++i)
//			{
//				// create variable
//				std::ostringstream s;
//				s << "X" << i;
//				Term var(
//						ID::MAINKIND_TERM |
//						ID::SUBKIND_TERM_VARIABLE |
//						ID::PROPERTY_AUX,
//						s.str());
//				const ID idvar = reg->storeConstOrVarTerm(var);
//				predAtom.tuple.push_back(idvar);
//				negpredAtom.tuple.push_back(idvar);
//			}
//
//			DBGLOG(DBG,"storing auxiliary atom " << predAtom);
//			idatom = reg->storeOrdinaryNAtom(predAtom);
//			DBGLOG(DBG,"storing auxiliary negative atom " << negpredAtom);
//			idnegatom = reg->storeOrdinaryNAtom(negpredAtom);
//		}
//
//		// create constraint
//		Rule r(
//				ID::MAINKIND_RULE |
//				ID::SUBKIND_RULE_CONSTRAINT |
//				ID::PROPERTY_AUX);
//
//		r.body.push_back(ID::posLiteralFromAtom(idatom));
//		r.body.push_back(ID::posLiteralFromAtom(idnegatom));
//
//		ID idcon = reg->storeRule(r);
//		ctx.idb.push_back(idcon);
//		DBGLOG(DBG,"created aux constraint '" <<
//				printToString<RawPrinter>(idcon, reg) << "'");
//	}

  }

} // anonymous namespace
//
//// rewrite program by adding auxiliary query rules
//PluginRewriterPtr ActionPlugin::createRewriter(ProgramCtx& ctx)
//{
//	ActionPlugin::CtxData& ctxdata = ctx.getPluginData<ActionPlugin>();
//	if( !ctxdata.enabled )
//		return PluginRewriterPtr();
//
//	return PluginRewriterPtr(new ActionPluginConstraintAdder);
//}
//
//namespace
//{
//
//class NegAuxPrinter:
//	public AuxPrinter
//{
//public:
//	typedef ActionPlugin::CtxData::NegToPosMap NegToPosMap;
//public:
//	NegAuxPrinter(
//			RegistryPtr reg,
//			PredicateMask& negAuxMask,
//			const NegToPosMap& ntpm):
//		reg(reg), mask(negAuxMask), ntpm(ntpm)
//	{
//	}
//
//  // print an ID and return true,
//  // or do not print it and return false
//  virtual bool print(std::ostream& out, ID id, const std::string& prefix) const
//	{
//		assert(id.isAuxiliary());
//		mask.updateMask();
//		DBGLOG(DBG,"mask is " << *mask.mask());
//		if( mask.mask()->getFact(id.address) )
//		{
//			// we cannot use any stored text to print this, we have to assemble it from pieces
//			DBGLOG(DBG,"printing auxiliary for strong negation: " << id);
//
//			// get replacement atom details
//			const OrdinaryAtom& r_atom = reg->ogatoms.getByAddress(id.address);
//
//			// find positive version of predicate
//			assert(!r_atom.tuple.empty());
//			const NegToPosMap::const_iterator itpred = ntpm.find(r_atom.tuple.front());
//			assert(itpred != ntpm.end());
//			const ID idpred = itpred->second;
//
//			// print strong negation
//			out << prefix << '-';
//
//			// print tuple
//      RawPrinter printer(out, reg);
//      // predicate
//      printer.print(idpred);
//      if( r_atom.tuple.size() > 1 )
//      {
//        Tuple t(r_atom.tuple.begin()+1, r_atom.tuple.end());
//        out << "(";
//        printer.printmany(t,",");
//        out << ")";
//      }
//
//			return true;
//		}
//		return false;
//	}
//
//protected:
//	RegistryPtr reg;
//	PredicateMask& mask;
//	const NegToPosMap& ntpm;
//};
//
//} // anonymous namespace

//namespace {
//
//  class ActionAuxPrinter: public AuxPrinter {
//    public:
//      ActionAuxPrinter(RegistryPtr reg, PredicateMask& mask) :
//          reg(reg), mask(mask) {
//      }
//
//      // print an ID and return true,
//      // or do not print it and return false
//      virtual bool print(std::ostream& out, ID id, const std::string& prefix) const {
//        return false;
////		assert(id.isAuxiliary());
////		mask.updateMask();
////		DBGLOG(DBG,"mask is " << *mask.mask());
////		if( mask.mask()->getFact(id.address) )
////		{
////			// we cannot use any stored text to print this, we have to assemble it from pieces
////			DBGLOG(DBG,"printing auxiliary for strong negation: " << id);
////
////			// get replacement atom details
////			const OrdinaryAtom& r_atom = reg->ogatoms.getByAddress(id.address);
////
////			// find positive version of predicate
////			assert(!r_atom.tuple.empty());
////			const NegToPosMap::const_iterator itpred = ntpm.find(r_atom.tuple.front());
////			assert(itpred != ntpm.end());
////			const ID idpred = itpred->second;
////
////			// print strong negation
////			out << prefix << '-';
////
////			// print tuple
////      RawPrinter printer(out, reg);
////      // predicate
////      printer.print(idpred);
////      if( r_atom.tuple.size() > 1 )
////      {
////        Tuple t(r_atom.tuple.begin()+1, r_atom.tuple.end());
////        out << "(";
////        printer.printmany(t,",");
////        out << ")";
////      }
////
////			return true;
////		}
////		return false;
//      }
//
//    protected:
//      RegistryPtr reg;
//      PredicateMask& mask;
//  };
//
//} // anonymous namespace

void ActionPlugin::setupProgramCtx(ProgramCtx& ctx) {
  ActionPlugin::CtxData& ctxdata = ctx.getPluginData<ActionPlugin>();
  if (!ctxdata.enabled)
    return;

  RegistryPtr reg = ctx.registry();

  // init predicate mask
  ctxdata.myAuxiliaryPredicateMask.setRegistry(reg);

  ActionPluginModelCallback * actionPluginModelCallback = new ActionPluginModelCallback(ctxdata,
      reg);
#warning here we could try to only remove the default answer set printer
  ModelCallbackPtr mcb(actionPluginModelCallback);
  ctx.modelCallbacks.clear();
  ctx.modelCallbacks.push_back(mcb);

  FinalCallbackPtr finalCallbackPtr(new ActionPluginFinalCallback(ctxdata, reg));
  ctx.finalCallbacks.push_back(finalCallbackPtr);

//	// add all auxiliaries to mask (here we should already have parsed all of them)
//	typedef CtxData::NegToPosMap NegToPosMap;
//	NegToPosMap::const_iterator it;
//	for(it = ctxdata.negToPos.begin();
//			it != ctxdata.negToPos.end(); ++it)
//	{
//		ctxdata.myAuxiliaryPredicateMask.addPredicate(it->first);
//	}
//
//	// update predicate mask
//	ctxdata.myAuxiliaryPredicateMask.updateMask();
//
//	// create auxiliary printer using mask
//	AuxPrinterPtr negAuxPrinter(new NegAuxPrinter(
//				reg, ctxdata.myAuxiliaryPredicateMask, ctxdata.negToPos));
//	reg->registerUserAuxPrinter(negAuxPrinter);

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
