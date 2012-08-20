/**
 * @file ActionPluginParserModule.h
 * @author Stefano Germano
 *
 * @brief ...
 */

#ifndef ACTION_PLUGIN_PARSER_MODULE_H_
#define ACTION_PLUGIN_PARSER_MODULE_H_

#include "acthex/ActionPlugin.h"

#include "dlvhex2/HexGrammar.h"
#include "dlvhex2/Printer.h"
#include "dlvhex2/HexParserModule.h"

DLVHEX_NAMESPACE_BEGIN

namespace qi = boost::spirit::qi;

class ActionPluginParserModuleSemantics: public HexGrammarSemantics {
public:
	ActionPlugin::CtxData& ctxdata;

public:
	ActionPluginParserModuleSemantics(ProgramCtx& ctx) :
			HexGrammarSemantics(ctx), ctxdata(ctx.getPluginData<ActionPlugin>()) {
	}
	;

	// use SemanticActionBase to redirect semantic action call into globally
	// specializable sem<T> struct space
	struct actionPrefixAtom: SemanticActionBase<
			ActionPluginParserModuleSemantics, ID, actionPrefixAtom> {
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
	//    const boost::fusion::vector2<
	//      dlvhex::ID,
	//      boost::optional<boost::optional<std::vector<dlvhex::ID> > >
	//    >& source,
	//    ID& target)
	//  {
	//    typedef ActionPlugin::CtxData::PredicateArityMap PredicateArityMap;
	//
	//    RegistryPtr reg = mgr.ctx.registry();
	//
	//    // predicate
	//    const ID& idpred = boost::fusion::at_c<0>(source);
	//
	//    // create/get aux constant for idpred
	//    const ID idnegpred = reg->getAuxiliaryConstantSymbol('s', idpred);
	//
	//    // build atom with auxiliary (SUBKIND is initialized by createAtom())
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
	//    // store predicate with arity (ensure each predicate is used with only one arity)
	//    PredicateArityMap::const_iterator it =
	//      mgr.ctxdata.negPredicateArities.find(idpred);
	//    if( it != mgr.ctxdata.negPredicateArities.end() )
	//    {
	//      // verify
	//      if( it->second != atom.tuple.size() - 1 )
	//      {
	//        LOG(ERROR,"strongly negated predicate '" <<
	//            printToString<RawPrinter>(idpred, reg) <<
	//            "' encountered with arity " << it->second <<
	//            " before and with arity " << (atom.tuple.size()-1) << " now");
	//        throw FatalError("got strongly negated predicate with multiple arities");
	//      }
	//    }
	//    else
	//    {
	//      // store as new
	//      mgr.ctxdata.negPredicateArities[idpred] = atom.tuple.size() - 1;
	//      mgr.ctxdata.negToPos[idnegpred] = idpred;
	//      DBGLOG(DBG,"got strongly negated predicate " <<
	//          printToString<RawPrinter>(idpred, reg) << "/" <<
	//          idpred << " with arity " << atom.tuple.size() - 1);
	//    }
	//
	//    // create atom
	//    createAtom(reg, atom, target);
	//  }

	void operator()(ActionPluginParserModuleSemantics& mgr,
			const boost::fusion::vector5<dlvhex::ID,
					boost::optional<boost::optional<dlvhex::Tuple> >,
					std::string, boost::optional<dlvhex::ID>,
					boost::optional<
							boost::fusion::vector2<boost::optional<dlvhex::ID>,
									boost::optional<dlvhex::ID> > > > & source,
			ID& target) {

		RegistryPtr reg = mgr.ctx.registry();

		RawPrinter printer(std::cerr, reg);

		if (mgr.ctxdata.idActionMap.count(boost::fusion::at_c < 0 > (source))
				== 0) {

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
				printer.printmany(
						boost::fusion::at_c < 1 > (source).get().get(), ",");

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

			if (!!boost::fusion::at_c < 0
					> (boost::fusion::at_c < 4 > (source).get()))
				printer.print(
						boost::fusion::at_c < 0
								> (boost::fusion::at_c < 4 > (source).get()).get());

			std::cerr << ':';

			if (!!boost::fusion::at_c < 1
					> (boost::fusion::at_c < 4 > (source).get()))
				printer.print(
						boost::fusion::at_c < 1
								> (boost::fusion::at_c < 4 > (source).get()).get());

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
		if (!!boost::fusion::at_c < 1 > (source)
				&& !!boost::fusion::at_c < 1 > (source).get()) {

			printer.printmany(boost::fusion::at_c < 1 > (source).get().get(),
					",");

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

			if (!!boost::fusion::at_c < 0
					> (boost::fusion::at_c < 4 > (source).get()))
				printer.print(
						boost::fusion::at_c < 0
								> (boost::fusion::at_c < 4 > (source).get()).get());
			else
				std::cerr << '1';

			std::cerr << ',';

			if (!!boost::fusion::at_c < 1
					> (boost::fusion::at_c < 4 > (source).get()))
				printer.print(
						boost::fusion::at_c < 1
								> (boost::fusion::at_c < 4 > (source).get()).get());
			else
				std::cerr << '1';

		} else
			std::cerr << "0,0";

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

		if (!!boost::fusion::at_c < 1 > (source)
				&& !!boost::fusion::at_c < 1 > (source).get()) {
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
			tuple.push_back(mgr.ctxdata.id_default_precedence);

		if (!!boost::fusion::at_c < 4 > (source)) {

			if (!!boost::fusion::at_c < 0
					> (boost::fusion::at_c < 4 > (source).get()))
				tuple.push_back(
						boost::fusion::at_c < 0
								> (boost::fusion::at_c < 4 > (source).get()).get());
			else
				tuple.push_back(mgr.ctxdata.id_default_weight_with_level);

			if (!!boost::fusion::at_c < 1
					> (boost::fusion::at_c < 4 > (source).get()))
				tuple.push_back(
						boost::fusion::at_c < 1
								> (boost::fusion::at_c < 4 > (source).get()).get());
			else
				tuple.push_back(mgr.ctxdata.id_default_level_with_weight);

		} else {
			tuple.push_back(mgr.ctxdata.id_default_weight_without_level);
			tuple.push_back(mgr.ctxdata.id_default_level_without_weight);
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
						>> -(qi::lit('[') >> -Base::terms >> qi::lit(']'))
						>> qi::lit('{')
						>> (qi::string("b") | qi::string("c_p")
								| qi::string("c"))
						>> -(qi::lit(',') >> Base::term) >> qi::lit('}')
						>> -(qi::lit('[') >> -(Base::term) >> qi::lit(':')
								>> -(Base::term) >> qi::lit(']')))[Sem::actionPrefixAtom(
						sem)];

#ifdef BOOST_SPIRIT_DEBUG
		BOOST_SPIRIT_DEBUG_NODE(actionPrefixAtom);
#endif
	}

	qi::rule<Iterator, ID(), Skipper> actionPrefixAtom;
};

struct ActionPluginParserModuleGrammar: ActionPluginParserModuleGrammarBase<
		HexParserIterator, HexParserSkipper>,
// required for interface
// note: HexParserModuleGrammar =
//       boost::spirit::qi::grammar<HexParserIterator, HexParserSkipper>
		HexParserModuleGrammar {
	typedef ActionPluginParserModuleGrammarBase<HexParserIterator,
			HexParserSkipper> GrammarBase;
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

DLVHEX_NAMESPACE_END

#endif /* ACTION_PLUGIN_PARSER_MODULE_H_ */
