/**
 * @file ActionParserModule.h
 * @author Stefano Germano
 *
 * @brief Parser for the Actions
 */

#ifndef ACTION_PARSER_MODULE_H_
#define ACTION_PARSER_MODULE_H_

#include "acthex/ActionPluginCtxData.h"

#include "dlvhex2/HexGrammar.h"
#include "dlvhex2/Printer.h"
#include "dlvhex2/HexParserModule.h"

DLVHEX_NAMESPACE_BEGIN

namespace qi = boost::spirit::qi;

class ActionParserModuleSemantics: public HexGrammarSemantics {
public:
	ActionPluginCtxData& ctxData;

public:
	ActionParserModuleSemantics(ProgramCtx& ctx);

	// use SemanticActionBase to redirect semantic action call into globally
	// specializable sem<T> struct space
	struct actionPrefixAtom: SemanticActionBase<
			ActionParserModuleSemantics, ID, actionPrefixAtom> {
		actionPrefixAtom(ActionParserModuleSemantics& mgr) :
				actionPrefixAtom::base_type(mgr) {
		}
	};
};

// create semantic handler for above semantic action
// (needs to be in globally specializable struct space)
template<>
struct sem<ActionParserModuleSemantics::actionPrefixAtom> {
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

	void operator()(ActionParserModuleSemantics& mgr,
			const boost::fusion::vector5<dlvhex::ID,
					boost::optional<boost::optional<dlvhex::Tuple> >,
					std::string, boost::optional<dlvhex::ID>,
					boost::optional<
							boost::fusion::vector2<boost::optional<dlvhex::ID>,
									boost::optional<dlvhex::ID> > > > & source,
			ID& target) {

		RegistryPtr reg = mgr.ctx.registry();

		RawPrinter printer(std::cerr, reg);

		const ID id_0 = boost::fusion::at_c < 0 > (source);

		std::cerr << "\nThe IdActionMap:" << std::endl;
		std::map<ID, ActionPtr>::iterator itIAM;
		for (itIAM = mgr.ctxData.idActionMap.begin();
				itIAM != mgr.ctxData.idActionMap.end(); itIAM++) {
			printer.print(itIAM->first);
			std::cerr << "\t\t";
			printer.print(itIAM->second->getAuxId());
			std::cerr << "\t\t";
			std::cerr << itIAM->second->getPredicate() << std::endl;
		}

		std::cerr << std::endl;

		if (mgr.ctxData.idActionMap.count(id_0) == 0)
			throw PluginError(
					"Action '" + reg->getTermStringByID(id_0) + "' not found");

		std::cerr << "original:\t";

		std::cerr << '#';

		printer.print(id_0);

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

		printer.print(mgr.ctxData.idActionMap.find(id_0)->second->getAuxId());

		std::cerr << '(';

		printer.print(id_0);

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

		tuple.push_back(mgr.ctxData.idActionMap.find(id_0)->second->getAuxId());
		tuple.push_back(id_0);

		if (!!boost::fusion::at_c < 1 > (source)
				&& !!boost::fusion::at_c < 1 > (source).get()) {
			const Tuple& terms = boost::fusion::at_c < 1 > (source).get().get();
			tuple.insert(tuple.end(), terms.begin(), terms.end());
		}

		if (boost::fusion::at_c < 2 > (source) == "b")
			tuple.push_back(mgr.ctxData.id_brave);
		else if (boost::fusion::at_c < 2 > (source) == "c")
			tuple.push_back(mgr.ctxData.id_cautious);
		else if (boost::fusion::at_c < 2 > (source) == "c_p")
			tuple.push_back(mgr.ctxData.id_preferred_cautious);
		else {
			assert(true);
		}

		if (!!boost::fusion::at_c < 3 > (source))
			tuple.push_back(boost::fusion::at_c < 3 > (source).get());
		else
			tuple.push_back(mgr.ctxData.id_default_precedence);

		if (!!boost::fusion::at_c < 4 > (source)) {

			if (!!boost::fusion::at_c < 0
					> (boost::fusion::at_c < 4 > (source).get()))
				tuple.push_back(
						boost::fusion::at_c < 0
								> (boost::fusion::at_c < 4 > (source).get()).get());
			else
				tuple.push_back(mgr.ctxData.id_default_weight_with_level);

			if (!!boost::fusion::at_c < 1
					> (boost::fusion::at_c < 4 > (source).get()))
				tuple.push_back(
						boost::fusion::at_c < 1
								> (boost::fusion::at_c < 4 > (source).get()).get());
			else
				tuple.push_back(mgr.ctxData.id_default_level_with_weight);

		} else {
			tuple.push_back(mgr.ctxData.id_default_weight_without_level);
			tuple.push_back(mgr.ctxData.id_default_level_without_weight);
		}

		createAtom(reg, oatom, target);

	}
};

namespace {

template<typename Iterator, typename Skipper>
struct ActionParserModuleGrammarBase:
// we derive from the original hex grammar
// -> we can reuse its rules
public HexGrammarBase<Iterator, Skipper> {
	typedef HexGrammarBase<Iterator, Skipper> Base;

	ActionParserModuleSemantics& sem;

	ActionParserModuleGrammarBase(ActionParserModuleSemantics& sem) :
			Base(sem), sem(sem) {
		typedef ActionParserModuleSemantics Sem;
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

struct ActionParserModuleGrammar: ActionParserModuleGrammarBase<
		HexParserIterator, HexParserSkipper>,
// required for interface
// note: HexParserModuleGrammar =
//       boost::spirit::qi::grammar<HexParserIterator, HexParserSkipper>
		HexParserModuleGrammar {
	typedef ActionParserModuleGrammarBase<HexParserIterator,
			HexParserSkipper> GrammarBase;
	typedef HexParserModuleGrammar QiBase;

	ActionParserModuleGrammar(ActionParserModuleSemantics& sem) :
			GrammarBase(sem), QiBase(GrammarBase::actionPrefixAtom) {
	}
};
typedef boost::shared_ptr<ActionParserModuleGrammar> ActionParserModuleGrammarPtr;

// moduletype = HexParserModule::HEADATOM
template<enum HexParserModule::Type moduletype>
class ActionParserModule: public HexParserModule {
public:
	// the semantics manager is stored/owned by this module!
	ActionParserModuleSemantics sem;
	// we also keep a shared ptr to the grammar module here
	ActionParserModuleGrammarPtr grammarModule;

	ActionParserModule(ProgramCtx& ctx) :
			HexParserModule(moduletype), sem(ctx) {
		LOG(INFO, "constructed ActionParserModule");
	}

	virtual HexParserModuleGrammarPtr createGrammarModule() {
		assert(
				!grammarModule
						&& "for simplicity (storing only one grammarModule pointer) we currently assume this will be called only once .. should be no problem to extend");
		grammarModule.reset(new ActionParserModuleGrammar(sem));
		LOG(INFO, "created ActionParserModuleGrammar");
		return grammarModule;
	}
};

} // anonymous namespace

DLVHEX_NAMESPACE_END

#endif /* ACTION_PARSER_MODULE_H_ */
