/**
 * @file ActionPluginParserModuleForBuiltInConstants.h
 * @author Stefano Germano
 *
 * @brief Parser for the Built in Constants
 */

#ifndef ACTION_PLUGIN_PARSER_MODULE_FOR_BUILT_IN_CONSTANTS_H_
#define ACTION_PLUGIN_PARSER_MODULE_FOR_BUILT_IN_CONSTANTS_H_

#include "acthex/ActionPlugin.h"

#include "dlvhex2/HexGrammar.h"
#include "dlvhex2/Printer.h"
#include "dlvhex2/HexParserModule.h"

DLVHEX_NAMESPACE_BEGIN

namespace qi = boost::spirit::qi;

class ActionPluginParserModuleSemanticsForBuiltInConstants: public HexGrammarSemantics {
public:
	ActionPlugin::CtxData& ctxdata;

public:
	ActionPluginParserModuleSemanticsForBuiltInConstants(ProgramCtx& ctx) :
			HexGrammarSemantics(ctx), ctxdata(ctx.getPluginData<ActionPlugin>()) {
	}
	;

	// use SemanticActionBase to redirect semantic action call into globally
	// specializable sem<T> struct space
	struct builtInConstantsPrefixAtom: SemanticActionBase<
			ActionPluginParserModuleSemanticsForBuiltInConstants, ID,
			builtInConstantsPrefixAtom> {
		builtInConstantsPrefixAtom(
				ActionPluginParserModuleSemanticsForBuiltInConstants& mgr) :
				builtInConstantsPrefixAtom::base_type(mgr) {
		}
	};
};

// create semantic handler for above semantic action
// (needs to be in globally specializable struct space)
template<>
struct sem<
		ActionPluginParserModuleSemanticsForBuiltInConstants::builtInConstantsPrefixAtom> {
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

	void operator()(ActionPluginParserModuleSemanticsForBuiltInConstants& mgr,
			const boost::fusion::vector3<std::string, std::string, const unsigned int> & source,
			ID& target) {

		mgr.ctxdata.addNumberIterations(boost::fusion::at_c < 2 > (source), mgr.ctx);

	}
};

namespace {

template<typename Iterator, typename Skipper>
struct ActionPluginParserModuleGrammarBaseForBuiltInConstants:
// we derive from the original hex grammar
// -> we can reuse its rules
public HexGrammarBase<Iterator, Skipper> {
	typedef HexGrammarBase<Iterator, Skipper> Base;

	ActionPluginParserModuleSemanticsForBuiltInConstants& sem;

	ActionPluginParserModuleGrammarBaseForBuiltInConstants(
			ActionPluginParserModuleSemanticsForBuiltInConstants& sem) :
			Base(sem), sem(sem) {
		typedef ActionPluginParserModuleSemanticsForBuiltInConstants Sem;
		builtInConstantsPrefixAtom =
				(qi::string("#acthex") >> qi::string("NumberIterations") >> qi::lit('=')
								>> qi::uint_/*
								| (qi::string("DurationIterations")
										>> qi::lit('=')
										>> *(qi::char_("0-9:,.")))*/
						>> qi::lit('.'))[Sem::builtInConstantsPrefixAtom(sem)];

#ifdef BOOST_SPIRIT_DEBUG
		BOOST_SPIRIT_DEBUG_NODE(builtInConstantsPrefixAtom);
#endif
	}

	qi::rule<Iterator, ID(), Skipper> builtInConstantsPrefixAtom;
};

struct ActionPluginParserModuleGrammarForBuiltInConstants: ActionPluginParserModuleGrammarBaseForBuiltInConstants<
		HexParserIterator, HexParserSkipper>,
// required for interface
// note: HexParserModuleGrammar =
//       boost::spirit::qi::grammar<HexParserIterator, HexParserSkipper>
		HexParserModuleGrammar {
	typedef ActionPluginParserModuleGrammarBaseForBuiltInConstants<
			HexParserIterator, HexParserSkipper> GrammarBase;
	typedef HexParserModuleGrammar QiBase;

	ActionPluginParserModuleGrammarForBuiltInConstants(
			ActionPluginParserModuleSemanticsForBuiltInConstants& sem) :
			GrammarBase(sem), QiBase(GrammarBase::builtInConstantsPrefixAtom) {
	}
};
typedef boost::shared_ptr<ActionPluginParserModuleGrammarForBuiltInConstants> ActionPluginParserModuleGrammarForBuiltInConstantsPtr;

// moduletype = HexParserModule::HEADATOM
template<enum HexParserModule::Type moduletype>
class ActionPluginParserModuleForBuiltInConstants: public HexParserModule {
public:
	// the semantics manager is stored/owned by this module!
	ActionPluginParserModuleSemanticsForBuiltInConstants sem;
	// we also keep a shared ptr to the grammar module here
	ActionPluginParserModuleGrammarForBuiltInConstantsPtr grammarModule;

	ActionPluginParserModuleForBuiltInConstants(ProgramCtx& ctx) :
			HexParserModule(moduletype), sem(ctx) {
		LOG(INFO, "constructed ActionPluginParserModuleForBuiltInConstants");
	}

	virtual HexParserModuleGrammarPtr createGrammarModule() {
		assert(
				!grammarModule
						&& "for simplicity (storing only one grammarModule pointer) we currently assume this will be called only once .. should be no problem to extend");
		grammarModule.reset(
				new ActionPluginParserModuleGrammarForBuiltInConstants(sem));
		LOG(INFO, "created ActionPluginParserModuleGrammar");
		return grammarModule;
	}
};

} // anonymous namespace

DLVHEX_NAMESPACE_END

#endif /* ACTION_PLUGIN_PARSER_MODULE_FOR_BUILT_IN_CONSTANTS_H_ */
