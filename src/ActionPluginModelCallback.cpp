/**
 * @file ActionPluginModelCallback.cpp
 * @author Stefano Germano
 *
 * @brief ...
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H
#include "acthex/ActionPluginModelCallback.h"

DLVHEX_NAMESPACE_BEGIN

ActionPluginModelCallback::ActionPluginModelCallback(CtxDataPtr ctxDataPtr,
		const RegistryPtr registryPtr) :
		ctxDataPtr(ctxDataPtr), registryPtr(registryPtr) {
}

ActionPluginModelCallback::~ActionPluginModelCallback() {
}

bool ActionPluginModelCallback::operator()(dlvhex::AnswerSetPtr answerSetPtr) {

	std::cerr << "\nActionPluginModelCallback called" << std::endl;
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

//  std::cerr << "Before" << std::endl;
//
//  ActionPlugin::CtxData& ctxData = ctx.getPluginData<ActionPlugin>();
//
//  std::cerr << "After" << std::endl;

	ActionPlugin::CtxData::LevelsAndWeights levelsAndWeights;

	ctxDataPtr->myAuxiliaryPredicateMask.updateMask();

	std::cerr << "\nThe Atoms Of Interpretation with myAuxiliaryPredicateMask:"
			<< std::endl;

	InterpretationPtr intr = InterpretationPtr(new Interpretation(registryPtr));
	intr->getStorage() |=
			ctxDataPtr->myAuxiliaryPredicateMask.mask()->getStorage();
	intr->getStorage() &= answerSetPtr->interpretation->getStorage();

	Interpretation::TrueBitIterator bit, bit_end;
	for (boost::tie(bit, bit_end) = intr->trueBits(); bit != bit_end; ++bit) {
		const OrdinaryAtom& oatom = registryPtr->ogatoms.getByAddress(*bit);
		//std::cerr << oatom << std::endl;
		std::cerr << "The atom: " << oatom.text << std::endl;

		const Tuple & tuple = oatom.tuple;

		int weight, level, weight_position = tuple.size() - 2, level_position =
				tuple.size() - 1;

		if (tuple[weight_position].isIntegerTerm()
				&& tuple[level_position].isIntegerTerm()) {

			weight = tuple[weight_position].address;
			level = tuple[level_position].address;

			std::cerr << "Weight: " << weight << std::endl;
			std::cerr << "Level: " << level << std::endl;

			if (levelsAndWeights.count(level) == 0)
				levelsAndWeights.insert(std::pair<int, int>(level, weight));
			else
				levelsAndWeights[level] += weight;

//      answerSetPtr->costWeight += tuple[tuple.size() - 2].address;
//      answerSetPtr->costLevel += tuple[tuple.size() - 1].address;

		} else {
			throw PluginError(
					"In ModelCallback Weight and/or Level aren't Integer term");
			//return false;
		}

	}

//  std::cerr << "\nWeight and Level after " << std::endl;
//  std::cerr << "Weight: " << answerSetPtr->costWeight << std::endl;
//  std::cerr << "Level: " << answerSetPtr->costLevel << std::endl;

	// eliminates eventual levels with weight 0 in levelsAndWeights
	ActionPlugin::CtxData::LevelsAndWeights::iterator itLAW =
			levelsAndWeights.begin();
	while (itLAW != levelsAndWeights.end())
		if (itLAW->second == 0)
			levelsAndWeights.erase(itLAW++);
		else
			++itLAW;

	if (levelsAndWeights.empty()) {
		// this is always a BestModel

		if (!ctxDataPtr->levelsAndWeightsBestModels.empty()) {
			//the AnswerSet has a "better" levelsAndWeights of the others AnswerSet in the bestModelsContainer
			ctxDataPtr->levelsAndWeightsBestModels = levelsAndWeights;
			ctxDataPtr->notBestModelsContainer.insert(
					ctxDataPtr->notBestModelsContainer.end(),
					ctxDataPtr->bestModelsContainer.begin(),
					ctxDataPtr->bestModelsContainer.end());
			ctxDataPtr->bestModelsContainer.clear();
		}

	} else if (ctxDataPtr->levelsAndWeightsBestModels.empty()) {

		if (!ctxDataPtr->bestModelsContainer.empty()) {
			ctxDataPtr->notBestModelsContainer.push_back(answerSetPtr);
			return true;
		}

		ctxDataPtr->levelsAndWeightsBestModels = levelsAndWeights;

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

		int res_is = isABestModel(ctxDataPtr->levelsAndWeightsBestModels,
				levelsAndWeights);

		std::cerr << "res_is " << res_is << std::endl;

		if (res_is == 0)
			;
		else if (res_is == -1) {
			ctxDataPtr->notBestModelsContainer.push_back(answerSetPtr);
			return true;
		} else if (res_is == 1) {
			//the AnswerSet has a "better" levelsAndWeights of the others AnswerSet in the bestModelsContainer
			ctxDataPtr->levelsAndWeightsBestModels = levelsAndWeights;
			ctxDataPtr->notBestModelsContainer.insert(
					ctxDataPtr->notBestModelsContainer.end(),
					ctxDataPtr->bestModelsContainer.begin(),
					ctxDataPtr->bestModelsContainer.end());
			ctxDataPtr->bestModelsContainer.clear();
		} else
			throw PluginError(
					"In ModelCallback, there was an error when operator() called isABestModel");

	}

	ctxDataPtr->bestModelsContainer.push_back(answerSetPtr);

	std::cerr << "\nThe levelsAndWeightsBestModels:" << std::endl;
	std::cerr << "Level\tWeight" << std::endl;
	for (itLAW = ctxDataPtr->levelsAndWeightsBestModels.begin();
			itLAW != ctxDataPtr->levelsAndWeightsBestModels.end(); itLAW++) {
		std::cerr << itLAW->first << '\t' << itLAW->second << std::endl;
	}

	std::cerr << "\nThe bestModelsContainer:" << std::endl;
	ActionPlugin::CtxData::BestModelsContainer::iterator itBMC;
	for (itBMC = ctxDataPtr->bestModelsContainer.begin();
			itBMC != ctxDataPtr->bestModelsContainer.end(); itBMC++) {
		(*itBMC)->interpretation->print(std::cerr);
		std::cerr << std::endl;
	}

	std::cerr << "\nThe notBestModelsContainer:" << std::endl;
	for (itBMC = ctxDataPtr->notBestModelsContainer.begin();
			itBMC != ctxDataPtr->notBestModelsContainer.end(); itBMC++) {
		(*itBMC)->interpretation->print(std::cerr);
		std::cerr << std::endl;
	}

	std::cerr << std::endl;

	return true;

}

// return 0 if it's a BestModel like the AnswerSets in bestModelsContainer
// return -1 if it isn't a BestModel
// return 1 if it's a BestModel and it's better than the AnswerSets in bestModelsContainer
int ActionPluginModelCallback::isABestModel(
		ActionPlugin::CtxData::LevelsAndWeights& levelsAndWeightsBestModels,
		ActionPlugin::CtxData::LevelsAndWeights& levelsAndWeights) {

	//CtxData::LevelsAndWeights::iterator it;
	// the last level that I've seen
	int lastLevelSeen = -1;

	std::cerr << "starting\t" << levelsAndWeights.size() << "\t"
			<< levelsAndWeightsBestModels.size() << std::endl;

	//if there is a level in levelsAndWeights that is greater than the highest level of levelsAndWeightsBestModels I have to return
	if ((*levelsAndWeights.rbegin()).first
			> (*levelsAndWeightsBestModels.rbegin()).first)
		return -1;

	std::cerr << "before for" << std::endl;

	ActionPlugin::CtxData::LevelsAndWeights::reverse_iterator ritLAW;
	int first, second;
	for (ritLAW = levelsAndWeightsBestModels.rbegin();
			ritLAW != levelsAndWeightsBestModels.rend(); ritLAW++) {
		//I need to sort the levelsAndWeightsBestModels for Level (I think that it's just sorted by key),
		//to inizialize lastLevelSeen at the highest level of levelsAndWeightsBestModels
		//and to compare, in descending order,the weights in levelsAndWeightsBestModels and the weights in levelsAndWeights

		first = ritLAW->first;
		second = ritLAW->second;

		std::cerr << "before if:\t" << first << "\t" << second << std::endl;

		//if there is a level in levelsAndWeights that is smallest than the lastLevelSeen and greater than the current level
		if (lastLevelSeen != -1)
			for (ActionPlugin::CtxData::LevelsAndWeights::reverse_iterator ritLAW2 =
					levelsAndWeights.rbegin();
					ritLAW2 != levelsAndWeights.rend(); ritLAW2++)
				if (ritLAW2->first < lastLevelSeen && ritLAW2->first > first)
					return -1;

		//if the level in levelsAndWeightsBestModels there isn't in levelsAndWeights I have to break
		if (levelsAndWeights.count(first) == 0)
			return 1;
		else {

			//if the level in levelsAndWeightsBestModels there is in levelsAndWeights but the weight in the second have a smallest value
			if ((levelsAndWeights.find(first))->second < second)
				return 1;
			else
			//if the level in levelsAndWeightsBestModels there is in levelsAndWeights but the weight in the second have a greater value
			if ((levelsAndWeights.find(first))->second > second)
				return -1;
			else
				//if the level in levelsAndWeightsBestModels there is in levelsAndWeights and the weights are the same I have to update lastLevelSeen
				lastLevelSeen = first;

		}
	}

	// if the execution arrives here this AnswerSet is a BestModel and
	//the AnswerSet has a the same levelsAndWeights of the others AnswerSet in the bestModelsContainer

	//if the size of levelsAndWeightsBestModels isn't the same of the size of levelsAndWeights
	if (levelsAndWeightsBestModels.size() != levelsAndWeights.size())
		return -1;

	return 0;

}

DLVHEX_NAMESPACE_END
