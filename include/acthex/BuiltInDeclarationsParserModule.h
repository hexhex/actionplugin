/**
 * @file BuiltInDeclarationsParserModule.h
 * @author Stefano Germano
 *
 * @brief Parser for the Built in Declarations
 */

#ifndef BUILT_IN_DECLARATIONS_PARSER_MODULE_H_
#define BUILT_IN_DECLARATIONS_PARSER_MODULE_H_

#include "acthex/ActionPluginCtxData.h"

#include "dlvhex2/HexGrammar.h"
#include "dlvhex2/Printer.h"
#include "dlvhex2/HexParserModule.h"

#include <boost/lexical_cast.hpp>

DLVHEX_NAMESPACE_BEGIN

namespace qi = boost::spirit::qi;

class BuiltInDeclarationsParserModuleSemantics: public HexGrammarSemantics {
public:
	ActionPluginCtxData& ctxData;

public:
	BuiltInDeclarationsParserModuleSemantics(ProgramCtx& ctx);

	// use SemanticActionBase to redirect semantic action call into globally
	// specializable sem<T> struct space
	struct builtInDeclarationsPrefixAtom: SemanticActionBase<
	BuiltInDeclarationsParserModuleSemantics, ID,
	builtInDeclarationsPrefixAtom> {
		builtInDeclarationsPrefixAtom(
				BuiltInDeclarationsParserModuleSemantics& mgr) :
					builtInDeclarationsPrefixAtom::base_type(mgr) {
		}
	};
};

// create semantic handler for above semantic action
// (needs to be in globally specializable struct space)
template<>
struct sem<
BuiltInDeclarationsParserModuleSemantics::builtInDeclarationsPrefixAtom> {
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

//	void operator()(BuiltInDeclarationsParserModuleSemantics& mgr,
//			const boost::fusion::vector2<std::string,
//					boost::variant<
//							boost::fusion::vector2<std::string,
//									const unsigned int>, boost::fusion::vector2<std::string, const char> > >
//			/*const boost::fusion::vector2< std::string, const std::vector<char, std::allocator<char> > >*/& source,
//			ID& target) {
//
//		std::cerr << "\n\n\nAAAA\t" << boost::fusion::at_c < 1
//				> (source).which() << "\n\n\n" << std::endl;
//		if (boost::fusion::at_c < 1 > (source).which() == 0) {
//
//			//std::cerr << "\n\n" << boost::fusion::at_c < 0 > (boost::get<boost::fusion::vector2<std::string, const unsigned int> > (boost::fusion::at_c < 1 > (source))) << "\n" << std::endl;
//			std::string typeOfIteration = boost::fusion::at_c < 0
//					> (boost::get
//							< boost::fusion::vector2<std::string,
//									const unsigned int>
//							> (boost::fusion::at_c < 1 > (source)));
//
//			std::cerr << "\n\n" << typeOfIteration << "\n" << std::endl;
//
//			mgr.ctxdata.addNumberIterations(
//					boost::fusion::at_c < 1
//							> (boost::get
//									< boost::fusion::vector2<std::string,
//											const unsigned int>
//									> (boost::fusion::at_c < 1 > (source))),
//					mgr.ctx);
//		} else if (boost::fusion::at_c < 1 > (source).which() == 1) {
//
//			std::string typeOfIteration = boost::get < std::string
//					> (boost::fusion::at_c < 1 > (source));
//
//			std::cerr << "\n\n" << typeOfIteration << "\n" << std::endl;
//
//			//mgr.ctxdata.addDurationIterations
//		} else
//			throw PluginError("Built-in Declaration not found");
//
//	}
	void operator()(BuiltInDeclarationsParserModuleSemantics& mgr,
			const boost::fusion::vector3<std::string, std::string,
			/*boost::variant<const unsigned int, boost::fusion::vector2<const char, const char> /*>*/
			const unsigned int> & source, ID& target) {

		std::string typeOfIteration = boost::fusion::at_c < 1 > (source);

		std::cerr << "\n\n" << typeOfIteration << std::endl;

		if (typeOfIteration == "NumberIterations") {

//			if (boost::fusion::at_c < 2 > (source).which() == 1)
//				throw PluginError("Built-in Declaration not found");
//
//			const unsigned int number = boost::get<const unsigned int>(
//					boost::fusion::at_c < 2 > (source));
//
//			std::cerr << number << "\n" << std::endl;
//
//			mgr.ctxdata.addNumberIterations(number, mgr.ctx);

			const unsigned int number = boost::fusion::at_c < 2 > (source);
			std::cerr << number << "\n" << std::endl;
			mgr.ctxData.addNumberIterations(number, mgr.ctx, true);

		} else if (typeOfIteration == "DurationIterations") {

//			if (boost::fusion::at_c < 2 > (source).which() == 0) {
//				//number
//				const unsigned int duration = boost::get<const unsigned int>(
//						boost::fusion::at_c < 2 > (source));
//
//				std::cerr << duration << "\n" << std::endl;
//
//				mgr.ctxdata.addDurationIterations(
//						boost::lexical_cast < std::string > (duration));
//
//			} else {
//				//string
//				//std::string duration = boost::get < std::string> (boost::fusion::at_c < 2 > (source)));
//				//std::cerr << duration << "\n" << std::endl;
//				const char first = boost::fusion::at_c < 0 > (boost::get < boost::fusion::vector2<const char, const char>
//						> (boost::fusion::at_c < 2 > (source)));
//				std::cerr << first << "\n" << std::endl;
//
////				mgr.ctxdata.addDurationIterations(
////						boost::get<const std::vector<char, std::allocator<char> > >(
////								boost::fusion::at_c < 2 > (source)));
//			}

#warning Duration specified in seconds
			const unsigned int duration = boost::fusion::at_c < 2 > (source);
			std::cerr << duration << "\n" << std::endl;
			mgr.ctxData.addDurationIterations(duration, true);

		} else
			throw PluginError("Built-in Declaration not found");

	}

};

namespace {

template<typename Iterator, typename Skipper>
struct BuiltInDeclarationsParserModuleGrammarBase:
// we derive from the original hex grammar
// -> we can reuse its rules
public HexGrammarBase<Iterator, Skipper> {
	typedef HexGrammarBase<Iterator, Skipper> Base;

	BuiltInDeclarationsParserModuleSemantics& sem;

	BuiltInDeclarationsParserModuleGrammarBase(
			BuiltInDeclarationsParserModuleSemantics& sem) :
			Base(sem), sem(sem) {
		typedef BuiltInDeclarationsParserModuleSemantics Sem;
//		builtInDeclarationsPrefixAtom = //(qi::string("#acthex") >> +qi::char_("0-9:,.")
//				(qi::string("#acthex")
//						>> ((qi::string("NumberIterations") >> qi::lit('=')
//								>> qi::uint_)
//								| (qi::string("DurationIterations")
//									//	>> qi::lit('=')
//										//>> qi::char_("0-9:,.")
//										>> qi::char_("0-9") //>> qi::char_(":,.")
//								//>> qi::string("VALUE")
//								//>> +qi::char_("0-9:,.")
//								)) >> qi::lit('.'))[Sem::builtInDeclarationsPrefixAtom(
//						sem)];

		builtInDeclarationsPrefixAtom = (qi::string("#acthex")
				>> (qi::string("NumberIterations")
						| qi::string("DurationIterations"))
				//>> qi::lit('=') >> (qi::uint_ | qi::string("VALUE"))
				>> qi::lit('=')
				//>> (qi::uint_ | (qi::char_("0-9") >> qi::char_(":,.")))
				//>> +qi::char_("0-9:,.")
				>> qi::uint_ >> qi::lit('.'))[Sem::builtInDeclarationsPrefixAtom(
				sem)];

#ifdef BOOST_SPIRIT_DEBUG
		BOOST_SPIRIT_DEBUG_NODE(builtInDeclarationsPrefixAtom);
#endif
	}

	qi::rule<Iterator, ID(), Skipper> builtInDeclarationsPrefixAtom;
};

struct BuiltInDeclarationsParserModuleGrammar: BuiltInDeclarationsParserModuleGrammarBase<
		HexParserIterator, HexParserSkipper>,
// required for interface
// note: HexParserModuleGrammar =
//       boost::spirit::qi::grammar<HexParserIterator, HexParserSkipper>
		HexParserModuleGrammar {
	typedef BuiltInDeclarationsParserModuleGrammarBase<
			HexParserIterator, HexParserSkipper> GrammarBase;
	typedef HexParserModuleGrammar QiBase;

	BuiltInDeclarationsParserModuleGrammar(
			BuiltInDeclarationsParserModuleSemantics& sem) :
			GrammarBase(sem), QiBase(GrammarBase::builtInDeclarationsPrefixAtom) {
	}
};
typedef boost::shared_ptr<BuiltInDeclarationsParserModuleGrammar> BuiltInDeclarationsParserModuleGrammarPtr;

// moduletype = HexParserModule::HEADATOM
template<enum HexParserModule::Type moduletype>
class BuiltInDeclarationsParserModule: public HexParserModule {
public:
	// the semantics manager is stored/owned by this module!
	BuiltInDeclarationsParserModuleSemantics sem;
	// we also keep a shared ptr to the grammar module here
	BuiltInDeclarationsParserModuleGrammarPtr grammarModule;

	BuiltInDeclarationsParserModule(ProgramCtx& ctx) :
			HexParserModule(moduletype), sem(ctx) {
		LOG(INFO, "constructed BuiltInDeclarationsParserModule");
	}

	virtual HexParserModuleGrammarPtr createGrammarModule() {
		assert(
				!grammarModule
						&& "for simplicity (storing only one grammarModule pointer) we currently assume this will be called only once .. should be no problem to extend");
		grammarModule.reset(
				new BuiltInDeclarationsParserModuleGrammar(sem));
		LOG(INFO, "created BuiltInDeclarationsParserModuleGrammar");
		return grammarModule;
	}
};

} // anonymous namespace

DLVHEX_NAMESPACE_END

#endif /* BUILT_IN_DECLARATIONS_PARSER_MODULE_H_ */
