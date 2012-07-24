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

#include "dlvhex2/PlatformDefinitions.h"
#include "dlvhex2/ProgramCtx.h"
#include "dlvhex2/Registry.h"
#include "dlvhex2/Printer.h"
#include "dlvhex2/Printhelpers.h"
#include "dlvhex2/PredicateMask.h"
#include "dlvhex2/Logger.h"
#include "dlvhex2/HexParser.h"
#include "dlvhex2/HexParserModule.h"
#include "dlvhex2/HexGrammar.h"

DLVHEX_NAMESPACE_BEGIN

namespace spirit = boost::spirit;
namespace qi = boost::spirit::qi;

ActionPlugin::MyModelCallback::MyModelCallback(ProgramCtx& ctx) :
    ctx(ctx) {

}

bool ActionPlugin::MyModelCallback::operator()(AnswerSetPtr answerSetPtr) {

  std::cerr << "\nMyModelCallback called" << std::endl;
  std::cerr << "\nThe Interpretation:" << std::endl;
  answerSetPtr->interpretation->print(std::cerr);
  std::cerr << std::endl;

//  std::cerr << "\nWeight and Level before " << std::endl;
//  std::cerr << "Weight: " << answerSetPtr->costWeight << std::endl;
//  std::cerr << "Level: " << answerSetPtr->costLevel << std::endl;
//
//  if (answerSetPtr->costWeight == -1)
//    answerSetPtr->costWeight = 0;
//
//  if (answerSetPtr->costLevel == -1)
//    answerSetPtr->costLevel = 0;

//  std::cerr << "\nThe Atoms Of Interpretation:" << std::endl;
  //  Interpretation::TrueBitIterator bit, bit_end;
//  for (boost::tie(bit, bit_end) = answerSetPtr->interpretation->trueBits(); bit != bit_end; ++bit) {
//    const OrdinaryAtom& oatom = ctx.registry()->ogatoms.getByAddress(*bit);
//    std::cerr << oatom << std::endl;
//    std::cerr << "The atom: " << oatom.text << std::endl;
//  }

  ActionPlugin::CtxData& ctxData = ctx.getPluginData<ActionPlugin>();

  CtxData::LevelsAndWeights levelsAndWeights;

  ctxData.myAuxiliaryPredicateMask.updateMask();

  std::cerr << "\nThe Atoms Of Interpretation with myAuxiliaryPredicateMask:" << std::endl;

  InterpretationPtr intr = InterpretationPtr(new Interpretation(ctx.registry()));
  intr->getStorage() |= ctxData.myAuxiliaryPredicateMask.mask()->getStorage();
  intr->getStorage() &= answerSetPtr->interpretation->getStorage();

  Interpretation::TrueBitIterator bit, bit_end;
  for (boost::tie(bit, bit_end) = intr->trueBits(); bit != bit_end; ++bit) {
    const OrdinaryAtom& oatom = ctx.registry()->ogatoms.getByAddress(*bit);
    //std::cerr << oatom << std::endl;
    std::cerr << "The atom: " << oatom.text << std::endl;

    const Tuple & tuple = oatom.tuple;

    if (tuple[tuple.size() - 2].isIntegerTerm() && tuple[tuple.size() - 1].isIntegerTerm()) {

      std::cerr << "Weight: " << tuple[tuple.size() - 2].address << std::endl;
      std::cerr << "Level: " << tuple[tuple.size() - 1].address << std::endl;

      if (levelsAndWeights.count(tuple[tuple.size() - 1].address) == 0)
        levelsAndWeights.insert(
            std::pair<int, int>(tuple[tuple.size() - 1].address, tuple[tuple.size() - 2].address));
      else
        levelsAndWeights[tuple[tuple.size() - 1].address] += tuple[tuple.size() - 2].address;

//      answerSetPtr->costWeight += tuple[tuple.size() - 2].address;
//      answerSetPtr->costLevel += tuple[tuple.size() - 1].address;

    } else {
      throw PluginError("In ModelCallback Weight and/or Level aren't Integer term");
      //return false;
    }

  }

//  std::cerr << "\nWeight and Level after " << std::endl;
//  std::cerr << "Weight: " << answerSetPtr->costWeight << std::endl;
//  std::cerr << "Level: " << answerSetPtr->costLevel << std::endl;

  // eliminates eventual levels with weight 0 in levelsAndWeights
  CtxData::LevelsAndWeights::iterator itLAW = levelsAndWeights.begin();
  while (itLAW != levelsAndWeights.end())
    if (itLAW->second == 0)
      levelsAndWeights.erase(itLAW++);
    else
      ++itLAW;

  if (ctxData.levelsAndWeightsBestModels.empty()) {

    ctxData.levelsAndWeightsBestModels = levelsAndWeights;

  } else {

//    //CtxData::LevelsAndWeights::iterator it;
//    // the last level that I've seen
//    int lastLevelSeen = -1;
//
//    //if there is a level in levelsAndWeights that is greater than the highest level of levelsAndWeightsBestModels I have to return
//    if ((*levelsAndWeights.rbegin()).first > (*ctxData.levelsAndWeightsBestModels.rbegin()).first)
//      return true;
//
//    CtxData::LevelsAndWeights::reverse_iterator ritLAW;
//    // return if I find that this AnswerSet isn't a BestModel
//    // break if I find that this AnswerSet is "better" than other AnswerSets in bestModelsContainer
//    for (ritLAW = ctxData.levelsAndWeightsBestModels.rbegin();
//        ritLAW != ctxData.levelsAndWeightsBestModels.rend(); ritLAW++) {
//      //I need to sort the levelsAndWeightsBestModels for Level (I think that it's just sorted by key),
//      //to inizialize lastLevelSeen at the highest level of levelsAndWeightsBestModels
//      //and to compare, in descending order,the weights in levelsAndWeightsBestModels and the weights in levelsAndWeights
//
//      //if there is a level in levelsAndWeights that is smallest than the lastLevelSeen and greater than the current level I have to return
//      if (lastLevelSeen != -1)
//        for (CtxData::LevelsAndWeights::reverse_iterator ritLAW2 = levelsAndWeights.rbegin();
//            ritLAW2 != levelsAndWeights.rend(); ritLAW2++)
//          if (ritLAW2->first < lastLevelSeen && ritLAW2->first > ritLAW->first)
//            return true;
//
//      //if the level in levelsAndWeightsBestModels there isn't in levelsAndWeights I have to break
//      if (levelsAndWeights.count(ritLAW->first) == 0)
//        break;
//      else {
//
//        //if the level in levelsAndWeightsBestModels there is in levelsAndWeights but the weight in the second have a smallest value I have to break
//        if ((levelsAndWeights.find(ritLAW->first))->second < ritLAW->second)
//          break;
//        else
//        //if the level in levelsAndWeightsBestModels there is in levelsAndWeights but the weight in the second have a greater value I have to return
//        if ((levelsAndWeights.find(ritLAW->first))->second > ritLAW->second)
//          return true;
//        else
//          //if the level in levelsAndWeightsBestModels there is in levelsAndWeights and the weights are the same I have to update lastLevelSeen and continue
//          lastLevelSeen = ritLAW->first;
//
//      }
//
//    }
//
//    // if the execution arrives here this AnswerSet is a BestModel
//
//    if (itLAW == ctxData.levelsAndWeightsBestModels.end()) {
//      //the AnswerSet has a the same levelsAndWeights of the others AnswerSet in the bestModelsContainer
//
//      //if the size of levelsAndWeightsBestModels isn't the same of the size of levelsAndWeights I can return
//      if (ctxData.levelsAndWeightsBestModels.size() != levelsAndWeights.size())
//        return true;
//
//    } else {
//      //the AnswerSet has a "better" levelsAndWeights of the others AnswerSet in the bestModelsContainer
//
//      ctxData.levelsAndWeightsBestModels = levelsAndWeights;
//
//      ctxData.bestModelsContainer.clear();
//
//    }

    int res_is = ActionPlugin::MyModelCallback::isABestModel(ctxData, levelsAndWeights);

    if (res_is == 0)
      ;
    else if (res_is == -1)
      return true;
    else if (res_is == 1) {
      //the AnswerSet has a "better" levelsAndWeights of the others AnswerSet in the bestModelsContainer
      ctxData.levelsAndWeightsBestModels = levelsAndWeights;
      ctxData.bestModelsContainer.clear();
    } else
      throw PluginError("In ModelCallback, there was an error when operator() called isABestModel");

  }

  ctxData.bestModelsContainer.push_back(answerSetPtr);

  std::cerr << "\nThe levelsAndWeightsBestModels:" << std::endl;
  std::cerr << "Level\tWeight" << std::endl;
  for (itLAW = ctxData.levelsAndWeightsBestModels.begin();
      itLAW != ctxData.levelsAndWeightsBestModels.end(); itLAW++) {
    std::cerr << itLAW->first << '\t' << itLAW->second << std::endl;
  }

  std::cerr << "\nThe bestModelsContainer:" << std::endl;
  CtxData::BestModelsContainer::iterator itBMC;
  for (itBMC = ctxData.bestModelsContainer.begin(); itBMC != ctxData.bestModelsContainer.end();
      itBMC++) {
    (*itBMC)->interpretation->print(std::cerr);
    std::cerr << std::endl;
  }

  std::cerr << std::endl;

  return true;

}

// return 0 if it's a BestModel like the AnswerSets in bestModelsContainer
// return -1 if it isn't a BestModel
// return 1 if it's a BestModel and it's better than the AnswerSets in bestModelsContainer
int ActionPlugin::MyModelCallback::isABestModel(ActionPlugin::CtxData& ctxData,
    ActionPlugin::CtxData::LevelsAndWeights& levelsAndWeights) {

  //CtxData::LevelsAndWeights::iterator it;
  // the last level that I've seen
  int lastLevelSeen = -1;

  //if there is a level in levelsAndWeights that is greater than the highest level of levelsAndWeightsBestModels I have to return
  if ((*levelsAndWeights.rbegin()).first > (*ctxData.levelsAndWeightsBestModels.rbegin()).first)
    return -1;

  CtxData::LevelsAndWeights::reverse_iterator ritLAW;
  // return if I find that this AnswerSet isn't a BestModel
  // break if I find that this AnswerSet is "better" than other AnswerSets in bestModelsContainer
  for (ritLAW = ctxData.levelsAndWeightsBestModels.rbegin();
      ritLAW != ctxData.levelsAndWeightsBestModels.rend(); ritLAW++) {
    //I need to sort the levelsAndWeightsBestModels for Level (I think that it's just sorted by key),
    //to inizialize lastLevelSeen at the highest level of levelsAndWeightsBestModels
    //and to compare, in descending order,the weights in levelsAndWeightsBestModels and the weights in levelsAndWeights

    //if there is a level in levelsAndWeights that is smallest than the lastLevelSeen and greater than the current level
    if (lastLevelSeen != -1)
      for (CtxData::LevelsAndWeights::reverse_iterator ritLAW2 = levelsAndWeights.rbegin();
          ritLAW2 != levelsAndWeights.rend(); ritLAW2++)
        if (ritLAW2->first < lastLevelSeen && ritLAW2->first > ritLAW->first)
          return -1;

    //if the level in levelsAndWeightsBestModels there isn't in levelsAndWeights I have to break
    if (levelsAndWeights.count(ritLAW->first) == 0)
      return 1;
    else {

      //if the level in levelsAndWeightsBestModels there is in levelsAndWeights but the weight in the second have a smallest value
      if ((levelsAndWeights.find(ritLAW->first))->second < ritLAW->second)
        return 1;
      else
      //if the level in levelsAndWeightsBestModels there is in levelsAndWeights but the weight in the second have a greater value
      if ((levelsAndWeights.find(ritLAW->first))->second > ritLAW->second)
        return -1;
      else
        //if the level in levelsAndWeightsBestModels there is in levelsAndWeights and the weights are the same I have to update lastLevelSeen
        lastLevelSeen = ritLAW->first;

    }
  }

  // if the execution arrives here this AnswerSet is a BestModel and
  //the AnswerSet has a the same levelsAndWeights of the others AnswerSet in the bestModelsContainer

  //if the size of levelsAndWeightsBestModels isn't the same of the size of levelsAndWeights
  if (ctxData.levelsAndWeightsBestModels.size() != levelsAndWeights.size())
    return -1;

  return 0;

}

ActionPlugin::CtxData::CtxData() :
    enabled(false), idActionMap(), levelsAndWeightsBestModels(), bestModelsContainer()
//, action_atom_name("aux_action_atom")
//, negPredicateArities()
{
}

void ActionPlugin::CtxData::addAction(const ID & id, const Action & action) {

  idActionMap.insert(std::pair<ID, Action>(id, action));

  myAuxiliaryPredicateMask.addPredicate(action.getAuxId());

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

    ctxdata.id_default_priority = ID::termFromInteger(1);
    ctxdata.id_default_weight = ID::termFromInteger(1);
    ctxdata.id_default_level = ID::termFromInteger(1);

  }

}

class ActionPluginParserModuleSemantics: public HexGrammarSemantics {
  public:
    ActionPlugin::CtxData& ctxdata;

  public:
    ActionPluginParserModuleSemantics(ProgramCtx& ctx) :
        HexGrammarSemantics(ctx), ctxdata(ctx.getPluginData<ActionPlugin>()) {
    }

    // use SemanticActionBase to redirect semantic action call into globally
    // specializable sem<T> struct space
    struct actionPrefixAtom: SemanticActionBase<ActionPluginParserModuleSemantics, ID,
        actionPrefixAtom> {
        actionPrefixAtom(ActionPluginParserModuleSemantics& mgr) :
            actionPrefixAtom::base_type(mgr) {
        }
    };
};

// create semantic handler for above semantic action
// (needs to be in globally specializable struct space)
template<>
struct sem<ActionPluginParserModuleSemantics::actionPrefixAtom> {
    void createAtom(RegistryPtr reg, OrdinaryAtom& atom, ID& target) {
      // groundness
      DBGLOG(DBG, "checking groundness of tuple " << printrange(atom.tuple));
      IDKind kind = 0;
      BOOST_FOREACH(const ID& id, atom.tuple)
      {
        kind |= id.kind;
        // make this sure to make the groundness check work
        // (if we add "builtin constant terms" like #supremum we might have to change the above statement)
        assert((id.kind & ID::SUBKIND_MASK) != ID::SUBKIND_TERM_BUILTIN);
      }
      const bool ground = !(kind & ID::SUBKIND_TERM_VARIABLE);
      if (ground) {
        atom.kind |= ID::SUBKIND_ATOM_ORDINARYG;
        target = reg->storeOrdinaryGAtom(atom);
      } else {
        atom.kind |= ID::SUBKIND_ATOM_ORDINARYN;
        target = reg->storeOrdinaryNAtom(atom);
      }
      DBGLOG(DBG, "stored atom " << atom << " which got id " << target);
    }

//  void operator()(
//    ActionPluginParserModuleSemantics& mgr,
//		const boost::fusion::vector2<
//			dlvhex::ID,
//		  boost::optional<boost::optional<std::vector<dlvhex::ID> > >
//		>& source,
//    ID& target)
//  {
//		typedef ActionPlugin::CtxData::PredicateArityMap PredicateArityMap;
//
//    RegistryPtr reg = mgr.ctx.registry();
//
//    // predicate
//    const ID& idpred = boost::fusion::at_c<0>(source);
//
//		// create/get aux constant for idpred
//		const ID idnegpred = reg->getAuxiliaryConstantSymbol('s', idpred);
//
//		// build atom with auxiliary (SUBKIND is initialized by createAtom())
//    OrdinaryAtom atom(ID::MAINKIND_ATOM | ID::PROPERTY_AUX);
//    atom.tuple.push_back(idnegpred);
//
//    // arguments
//    if( (!!boost::fusion::at_c<1>(source)) &&
//        (!!(boost::fusion::at_c<1>(source).get())) )
//    {
//      const Tuple& tuple = boost::fusion::at_c<1>(source).get().get();
//      atom.tuple.insert(atom.tuple.end(), tuple.begin(), tuple.end());
//    }
//
//		// store predicate with arity (ensure each predicate is used with only one arity)
//		PredicateArityMap::const_iterator it =
//			mgr.ctxdata.negPredicateArities.find(idpred);
//		if( it != mgr.ctxdata.negPredicateArities.end() )
//		{
//			// verify
//			if( it->second != atom.tuple.size() - 1 )
//			{
//				LOG(ERROR,"strongly negated predicate '" <<
//						printToString<RawPrinter>(idpred, reg) <<
//						"' encountered with arity " << it->second <<
//						" before and with arity " << (atom.tuple.size()-1) << " now");
//				throw FatalError("got strongly negated predicate with multiple arities");
//			}
//		}
//		else
//		{
//			// store as new
//			mgr.ctxdata.negPredicateArities[idpred] = atom.tuple.size() - 1;
//			mgr.ctxdata.negToPos[idnegpred] = idpred;
//			DBGLOG(DBG,"got strongly negated predicate " <<
//					printToString<RawPrinter>(idpred, reg) << "/" <<
//					idpred << " with arity " << atom.tuple.size() - 1);
//		}
//
//		// create atom
//		createAtom(reg, atom, target);
//  }

    void operator()(ActionPluginParserModuleSemantics& mgr,
        const boost::fusion::vector5<dlvhex::ID, boost::optional<boost::optional<dlvhex::Tuple> >,
            std::string, boost::optional<dlvhex::ID>,
            boost::optional<
                boost::fusion::vector2<boost::optional<dlvhex::ID>, boost::optional<dlvhex::ID> > > > & source,
        ID& target) {

      RegistryPtr reg = mgr.ctx.registry();

      RawPrinter printer(std::cerr, reg);

      if (mgr.ctxdata.idActionMap.count(boost::fusion::at_c < 0 > (source)) == 0) {

        const ID id = boost::fusion::at_c < 0 > (source);

        ID aux_id = reg->getAuxiliaryConstantSymbol('a', id);

        Action action(reg->getTermStringByID(id), aux_id);

        mgr.ctxdata.addAction(id, action);
        //mgr.ctxdata.idActionMap.insert(std::pair<ID, Action>(id, action));

#warning the above code should be deleted, the one below should be uncommented

        //throw an exception
//        std::cerr << "Action not found" << std::endl;
//        return;

      }

//      const dlvhex::ID & id_predicate = boost::fusion::at_c < 0 > (source);
//
//      std::map<ID, Action> & idActionMap = mgr.ctxdata.idActionMap;
//
////      Action & action = idActionMap[id_predicate];
//      Action & action = idActionMap.find(id_predicate)->second;
//
////      printer.print(idActionMap[id_predicate].getAuxId());

//      printer.print(mgr.ctxdata.idActionMap.find(boost::fusion::at_c < 0 > (source))->second.getAuxId());

      std::cerr << "original:\t";

      std::cerr << '#';

      printer.print(boost::fusion::at_c < 0 > (source));

      if (!!boost::fusion::at_c < 1 > (source)) {

        std::cerr << '[';

        if (!!boost::fusion::at_c < 1 > (source).get())
          printer.printmany(boost::fusion::at_c < 1 > (source).get().get(), ",");

        std::cerr << ']';

      }

      std::cerr << '{';

      std::cerr << boost::fusion::at_c < 2 > (source);

      if (!!boost::fusion::at_c < 3 > (source)) {

        std::cerr << ',';

        printer.print(boost::fusion::at_c < 3 > (source).get());

      }

      std::cerr << '}';

      if (!!boost::fusion::at_c < 4 > (source)) {

        std::cerr << '[';

        if (!!boost::fusion::at_c < 0 > (boost::fusion::at_c < 4 > (source).get()))
          printer.print(boost::fusion::at_c < 0 > (boost::fusion::at_c < 4 > (source).get()).get());

        std::cerr << ':';

        if (!!boost::fusion::at_c < 1 > (boost::fusion::at_c < 4 > (source).get()))
          printer.print(boost::fusion::at_c < 0 > (boost::fusion::at_c < 4 > (source).get()).get());

        std::cerr << ']';

      }

      std::cerr << std::endl;

      std::cerr << "rewritten:\t";

//      std::cerr << reg->getTermStringByID(mgr.ctxdata.id_in_the_registry);

      printer.print(
          mgr.ctxdata.idActionMap.find(boost::fusion::at_c < 0 > (source))->second.getAuxId());

      std::cerr << '(';

      printer.print(boost::fusion::at_c < 0 > (source));

      std::cerr << ',';
      if (!!boost::fusion::at_c < 1 > (source) && !!boost::fusion::at_c < 1 > (source).get()) {

        printer.printmany(boost::fusion::at_c < 1 > (source).get().get(), ",");

        std::cerr << ',';

      }

      std::cerr << boost::fusion::at_c < 2 > (source);

      std::cerr << ',';

      if (!!boost::fusion::at_c < 3 > (source))
        printer.print(boost::fusion::at_c < 3 > (source).get());
      else
        std::cerr << '1';

      std::cerr << ',';

      if (!!boost::fusion::at_c < 4 > (source)) {

        if (!!boost::fusion::at_c < 0 > (boost::fusion::at_c < 4 > (source).get()))
          printer.print(boost::fusion::at_c < 0 > (boost::fusion::at_c < 4 > (source).get()).get());
        else
          std::cerr << '1';

        std::cerr << ',';

        if (!!boost::fusion::at_c < 1 > (boost::fusion::at_c < 4 > (source).get()))
          printer.print(boost::fusion::at_c < 0 > (boost::fusion::at_c < 4 > (source).get()).get());
        else
          std::cerr << '1';

      } else
        std::cerr << "1,1";

      std::cerr << ')';

      std::cerr << '.';

      std::cerr << std::endl;

      //Take the IDs, create OrdinaryAtom, store it in the Registry by using storeOrdinaryAtom and put in target the ID

      OrdinaryAtom oatom(ID::MAINKIND_ATOM | ID::PROPERTY_AUX);

      Tuple& tuple = oatom.tuple;

//      tuple.push_back(reg->getTermStringByID(mgr.ctxdata.id_in_the_registry));
      tuple.push_back(
          mgr.ctxdata.idActionMap.find(boost::fusion::at_c < 0 > (source))->second.getAuxId());
      tuple.push_back(boost::fusion::at_c < 0 > (source));

      if (!!boost::fusion::at_c < 1 > (source) && !!boost::fusion::at_c < 1 > (source).get()) {
        const Tuple& terms = boost::fusion::at_c < 1 > (source).get().get();
        tuple.insert(tuple.end(), terms.begin(), terms.end());
      }

      if (boost::fusion::at_c < 2 > (source) == "b")
        tuple.push_back(mgr.ctxdata.id_brave);
      else if (boost::fusion::at_c < 2 > (source) == "c")
        tuple.push_back(mgr.ctxdata.id_cautious);
      else if (boost::fusion::at_c < 2 > (source) == "c_p")
        tuple.push_back(mgr.ctxdata.id_preferred_cautious);
      else {
        assert(true);
      }

      if (!!boost::fusion::at_c < 3 > (source))
        tuple.push_back(boost::fusion::at_c < 3 > (source).get());
      else
        tuple.push_back(mgr.ctxdata.id_default_priority);

      if (!!boost::fusion::at_c < 4 > (source)) {

        if (!!boost::fusion::at_c < 0 > (boost::fusion::at_c < 4 > (source).get()))
          tuple.push_back(
              boost::fusion::at_c < 0 > (boost::fusion::at_c < 4 > (source).get()).get());
        else
          tuple.push_back(mgr.ctxdata.id_default_weight);

        if (!!boost::fusion::at_c < 1 > (boost::fusion::at_c < 4 > (source).get()))
          tuple.push_back(
              boost::fusion::at_c < 1 > (boost::fusion::at_c < 4 > (source).get()).get());
        else
          tuple.push_back(mgr.ctxdata.id_default_level);

      } else {
        tuple.push_back(mgr.ctxdata.id_default_weight);
        tuple.push_back(mgr.ctxdata.id_default_level);
      }

      createAtom(reg, oatom, target);

    }
};

namespace {

  template<typename Iterator, typename Skipper>
  struct ActionPluginParserModuleGrammarBase:
// we derive from the original hex grammar
// -> we can reuse its rules
  public HexGrammarBase<Iterator, Skipper> {
      typedef HexGrammarBase<Iterator, Skipper> Base;

      ActionPluginParserModuleSemantics& sem;

      ActionPluginParserModuleGrammarBase(ActionPluginParserModuleSemantics& sem) :
          Base(sem), sem(sem) {
        typedef ActionPluginParserModuleSemantics Sem;
        actionPrefixAtom =
            (qi::lit('#') >> Base::classicalAtomPredicate
                >> -(qi::lit('[') >> -Base::terms >> qi::lit(']')) >> qi::lit('{')
                >> (qi::string("b") | qi::string("c_p") | qi::string("c"))
                >> -(qi::lit(',') >> Base::term) >> qi::lit('}')
                >> -(qi::lit('[') >> -(Base::term) >> qi::lit(':') >> -(Base::term) >> qi::lit(']')))[Sem::actionPrefixAtom(
                sem)];

#ifdef BOOST_SPIRIT_DEBUG
        BOOST_SPIRIT_DEBUG_NODE(actionPrefixAtom);
#endif
      }

      qi::rule<Iterator, ID(), Skipper> actionPrefixAtom;
  };

  struct ActionPluginParserModuleGrammar: ActionPluginParserModuleGrammarBase<HexParserIterator,
      HexParserSkipper>,
// required for interface
// note: HexParserModuleGrammar =
//       boost::spirit::qi::grammar<HexParserIterator, HexParserSkipper>
      HexParserModuleGrammar {
      typedef ActionPluginParserModuleGrammarBase<HexParserIterator, HexParserSkipper> GrammarBase;
      typedef HexParserModuleGrammar QiBase;

      ActionPluginParserModuleGrammar(ActionPluginParserModuleSemantics& sem) :
          GrammarBase(sem), QiBase(GrammarBase::actionPrefixAtom) {
      }
  };
  typedef boost::shared_ptr<ActionPluginParserModuleGrammar> ActionPluginParserModuleGrammarPtr;

// moduletype = HexParserModule::HEADATOM
  template<enum HexParserModule::Type moduletype>
  class ActionPluginParserModule: public HexParserModule {
    public:
      // the semantics manager is stored/owned by this module!
      ActionPluginParserModuleSemantics sem;
      // we also keep a shared ptr to the grammar module here
      ActionPluginParserModuleGrammarPtr grammarModule;

      ActionPluginParserModule(ProgramCtx& ctx) :
          HexParserModule(moduletype), sem(ctx) {
        LOG(INFO, "constructed ActionPluginParserModule");
      }

      virtual HexParserModuleGrammarPtr createGrammarModule() {
        assert(
            !grammarModule
                && "for simplicity (storing only one grammarModule pointer) we currently assume this will be called only once .. should be no problem to extend");
        grammarModule.reset(new ActionPluginParserModuleGrammar(sem));
        LOG(INFO, "created ActionPluginParserModuleGrammar");
        return grammarModule;
      }
  };

} // anonymous namespace

// create parser modules that extend and the basic hex grammar
// this parser also stores the query information into the plugin
std::vector<HexParserModulePtr> ActionPlugin::createParserModules(ProgramCtx & ctx) {
  DBGLOG(DBG, "ActionPlugin::createParserModules()");
  std::vector < HexParserModulePtr > ret;

  ActionPlugin::CtxData& ctxdata = ctx.getPluginData<ActionPlugin>();
  if (ctxdata.enabled) {
    ret.push_back(HexParserModulePtr(new ActionPluginParserModule<HexParserModule::HEADATOM>(ctx)));
  }

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

namespace {

  class ActionAuxPrinter: public AuxPrinter {
    public:
      ActionAuxPrinter(RegistryPtr reg, PredicateMask& mask) :
          reg(reg), mask(mask) {
      }

      // print an ID and return true,
      // or do not print it and return false
      virtual bool print(std::ostream& out, ID id, const std::string& prefix) const {
        return false;
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
      }

    protected:
      RegistryPtr reg;
      PredicateMask& mask;
  };

} // anonymous namespace

// register auxiliary printer for strong negation auxiliaries
void ActionPlugin::setupProgramCtx(ProgramCtx& ctx) {
  ActionPlugin::CtxData& ctxdata = ctx.getPluginData<ActionPlugin>();
  if (!ctxdata.enabled)
    return;

  RegistryPtr reg = ctx.registry();

  // init predicate mask
  ctxdata.myAuxiliaryPredicateMask.setRegistry(reg);

  MyModelCallback * myModelCallback = new MyModelCallback(ctx);
#warning here we could try to only remove the default answer set printer
  ModelCallbackPtr mcb(myModelCallback);
  ctx.modelCallbacks.clear();
  ctx.modelCallbacks.push_back(mcb);

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
