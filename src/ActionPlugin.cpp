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
 * @brief Plugin for cautions/brave ground/nonground queries in dlvhex.
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

ActionPlugin::CtxData::CtxData():
	enabled(false),
	negPredicateArities()
{
}

ActionPlugin::ActionPlugin():
	PluginInterface()
{
	setNameVersion("dlvhex-actionplugin[internal]", 2, 0, 0);
}

ActionPlugin::~ActionPlugin()
{
}

// output help message for this plugin
void ActionPlugin::printUsage(std::ostream& o) const
{
  //    123456789-123456789-123456789-123456789-123456789-123456789-123456789-123456789-
	o << "     --action-enable   Enable strong negation plugin." << std::endl;
}

// accepted options: --action-enable
//
// processes options for this plugin, and removes recognized options from pluginOptions
// (do not free the pointers, the const char* directly come from argv)
void ActionPlugin::processOptions(
		std::list<const char*>& pluginOptions,
		ProgramCtx& ctx)
{
	ActionPlugin::CtxData& ctxdata = ctx.getPluginData<ActionPlugin>();

	typedef std::list<const char*>::iterator Iterator;
	Iterator it;
	#warning create (or reuse, maybe from potassco?) cmdline option processing facility
	it = pluginOptions.begin();
	while( it != pluginOptions.end() )
	{
		bool processed = false;
		const std::string str(*it);
		if( str == "--action-enable" )
		{
			ctxdata.enabled = true;
			processed = true;
		}

		if( processed )
		{
			// return value of erase: element after it, maybe end()
			DBGLOG(DBG,"ActionPlugin successfully processed option " << str);
			it = pluginOptions.erase(it);
		}
		else
		{
			it++;
		}
	}
}
	
class ActionPluginParserModuleSemantics:
	public HexGrammarSemantics
{
public:
	ActionPlugin::CtxData& ctxdata;

public:
	ActionPluginParserModuleSemantics(ProgramCtx& ctx):
		HexGrammarSemantics(ctx),
		ctxdata(ctx.getPluginData<ActionPlugin>())
	{
	}

	// use SemanticActionBase to redirect semantic action call into globally
	// specializable sem<T> struct space
	struct actionPrefixAtom:
		SemanticActionBase<ActionPluginParserModuleSemantics, ID, actionPrefixAtom>
	{
		actionPrefixAtom(ActionPluginParserModuleSemantics& mgr):
			actionPrefixAtom::base_type(mgr)
		{
		}
	};
};

// create semantic handler for above semantic action
// (needs to be in globally specializable struct space)
template<>
struct sem<ActionPluginParserModuleSemantics::actionPrefixAtom>
{
  void createAtom(RegistryPtr reg, OrdinaryAtom& atom, ID& target)
  {
    // groundness
    DBGLOG(DBG,"checking groundness of tuple " << printrange(atom.tuple));
    IDKind kind = 0;
    BOOST_FOREACH(const ID& id, atom.tuple)
    {
      kind |= id.kind;
      // make this sure to make the groundness check work
      // (if we add "builtin constant terms" like #supremum we might have to change the above statement)
      assert((id.kind & ID::SUBKIND_MASK) != ID::SUBKIND_TERM_BUILTIN);
    }
    const bool ground = !(kind & ID::SUBKIND_TERM_VARIABLE);
    if( ground )
    {
      atom.kind |= ID::SUBKIND_ATOM_ORDINARYG;
      target = reg->storeOrdinaryGAtom(atom);
    }
    else
    {
      atom.kind |= ID::SUBKIND_ATOM_ORDINARYN;
      target = reg->storeOrdinaryNAtom(atom);
    }
    DBGLOG(DBG,"stored atom " << atom << " which got id " << target);
  }

  void operator()(
    ActionPluginParserModuleSemantics& mgr,
		const boost::fusion::vector2<
			dlvhex::ID,
		  boost::optional<boost::optional<std::vector<dlvhex::ID> > >
		>& source,
    ID& target)
  {
		typedef ActionPlugin::CtxData::PredicateArityMap PredicateArityMap;

    RegistryPtr reg = mgr.ctx.registry();

		// strong negation is always present here!

    // predicate
    const ID& idpred = boost::fusion::at_c<0>(source);

		// create/get aux constant for idpred
		const ID idnegpred = reg->getAuxiliaryConstantSymbol('s', idpred);

		// build atom with auxiliary (SUBKIND is initialized by createAtom())
    OrdinaryAtom atom(ID::MAINKIND_ATOM | ID::PROPERTY_AUX);
    atom.tuple.push_back(idnegpred);

    // arguments
    if( (!!boost::fusion::at_c<1>(source)) &&
        (!!(boost::fusion::at_c<1>(source).get())) )
    {
      const Tuple& tuple = boost::fusion::at_c<1>(source).get().get();
      atom.tuple.insert(atom.tuple.end(), tuple.begin(), tuple.end());
    }

		// store predicate with arity (ensure each predicate is used with only one arity)
		PredicateArityMap::const_iterator it =
			mgr.ctxdata.negPredicateArities.find(idpred);
		if( it != mgr.ctxdata.negPredicateArities.end() )
		{
			// verify
			if( it->second != atom.tuple.size() - 1 )
			{
				LOG(ERROR,"strongly negated predicate '" <<
						printToString<RawPrinter>(idpred, reg) <<
						"' encountered with arity " << it->second <<
						" before and with arity " << (atom.tuple.size()-1) << " now");
				throw FatalError("got strongly negated predicate with multiple arities");
			}
		}
		else
		{
			// store as new
			mgr.ctxdata.negPredicateArities[idpred] = atom.tuple.size() - 1;
			mgr.ctxdata.negToPos[idnegpred] = idpred;
			DBGLOG(DBG,"got strongly negated predicate " <<
					printToString<RawPrinter>(idpred, reg) << "/" <<
					idpred << " with arity " << atom.tuple.size() - 1);
		}

		// create atom
		createAtom(reg, atom, target);
  }
};

namespace
{

template<typename Iterator, typename Skipper>
struct ActionPluginParserModuleGrammarBase:
	// we derive from the original hex grammar
	// -> we can reuse its rules
	public HexGrammarBase<Iterator, Skipper>
{
	typedef HexGrammarBase<Iterator, Skipper> Base;

	ActionPluginParserModuleSemantics& sem;

	ActionPluginParserModuleGrammarBase(ActionPluginParserModuleSemantics& sem):
		Base(sem),
		sem(sem)
	{
		typedef ActionPluginParserModuleSemantics Sem;
		actionPrefixAtom
			= (
					qi::lit('#') >> Base::classicalAtomPredicate >>
					qi::lit('[') >> -Base::terms >> qi::lit(']') >>
					qi::lit('{') >> (qi::lit('b') | qi::lit('c') | qi::lit("c_p")) >> -(qi::lit(',') >> Base::term) >> qi::lit('}') >>
					-(qi::lit('[') >> -(Base::term) >> qi::lit(':') >> -(Base::term) >> qi::lit(']'))
				) [ Sem::actionPrefixAtom(sem) ];

		#ifdef BOOST_SPIRIT_DEBUG
		BOOST_SPIRIT_DEBUG_NODE(actionPrefixAtom);
		#endif
	}

	qi::rule<Iterator, ID(), Skipper> actionPrefixAtom;
};

struct ActionPluginParserModuleGrammar:
  ActionPluginParserModuleGrammarBase<HexParserIterator, HexParserSkipper>,
	// required for interface
  // note: HexParserModuleGrammar =
	//       boost::spirit::qi::grammar<HexParserIterator, HexParserSkipper>
	HexParserModuleGrammar
{
	typedef ActionPluginParserModuleGrammarBase<HexParserIterator, HexParserSkipper> GrammarBase;
  typedef HexParserModuleGrammar QiBase;

  ActionPluginParserModuleGrammar(ActionPluginParserModuleSemantics& sem):
    GrammarBase(sem),
    QiBase(GrammarBase::actionPrefixAtom)
  {
  }
};
typedef boost::shared_ptr<ActionPluginParserModuleGrammar>
	ActionPluginParserModuleGrammarPtr;

// moduletype = HexParserModule::BODYATOM
// moduletype = HexParserModule::HEADATOM
template<enum HexParserModule::Type moduletype>
class ActionPluginParserModule:
	public HexParserModule
{
public:
	// the semantics manager is stored/owned by this module!
	ActionPluginParserModuleSemantics sem;
	// we also keep a shared ptr to the grammar module here
	ActionPluginParserModuleGrammarPtr grammarModule;

	ActionPluginParserModule(ProgramCtx& ctx):
		HexParserModule(moduletype),
		sem(ctx)
	{
		LOG(INFO,"constructed ActionPluginParserModule");
	}

	virtual HexParserModuleGrammarPtr createGrammarModule()
	{
		assert(!grammarModule && "for simplicity (storing only one grammarModule pointer) we currently assume this will be called only once .. should be no problem to extend");
		grammarModule.reset(new ActionPluginParserModuleGrammar(sem));
		LOG(INFO,"created ActionPluginParserModuleGrammar");
		return grammarModule;
	}
};

} // anonymous namespace

// create parser modules that extend and the basic hex grammar
// this parser also stores the query information into the plugin
std::vector<HexParserModulePtr>
ActionPlugin::createParserModules(ProgramCtx& ctx)
{
	DBGLOG(DBG,"ActionPlugin::createParserModules()");
	std::vector<HexParserModulePtr> ret;

	ActionPlugin::CtxData& ctxdata = ctx.getPluginData<ActionPlugin>();
	if( ctxdata.enabled )
	{
		ret.push_back(HexParserModulePtr(
					new ActionPluginParserModule<HexParserModule::HEADATOM>(ctx)));
	}

	return ret;
}

namespace
{

typedef ActionPlugin::CtxData CtxData;

class ActionPluginConstraintAdder:
	public PluginRewriter
{
public:
	ActionPluginConstraintAdder() {}
	virtual ~ActionPluginConstraintAdder() {}

  virtual void rewrite(ProgramCtx& ctx);
};

void ActionPluginConstraintAdder::rewrite(ProgramCtx& ctx)
{
	typedef ActionPlugin::CtxData::PredicateArityMap PredicateArityMap;

	DBGLOG_SCOPE(DBG,"neg_rewr",false);
	DBGLOG(DBG,"= ActionPluginConstraintAdder::rewrite");

	ActionPlugin::CtxData& ctxdata = ctx.getPluginData<ActionPlugin>();
	assert(ctxdata.enabled && "this rewriter should only be used "
			"if the plugin is enabled");

	RegistryPtr reg = ctx.registry();
	assert(reg);
	PredicateArityMap::const_iterator it;
	for(it = ctxdata.negPredicateArities.begin();
			it != ctxdata.negPredicateArities.end(); ++it)
	{
		// for predicate foo of arity k create constraint
		// :- foo(X1,X2,...,Xk), foo_neg_aux(X1,X2,...,Xk).

		// create atoms
		const ID idpred = it->first;
		const unsigned arity = it->second;
		DBGLOG(DBG,"processing predicate '" <<
				printToString<RawPrinter>(idpred, reg) << "'/" << idpred <<
				" with arity " << arity);

		const ID idnegpred = reg->getAuxiliaryConstantSymbol('s', idpred);
		ID idatom;
		ID idnegatom;
		if( arity == 0 )
		{
			// ground atoms
			OrdinaryAtom predAtom(
					ID::MAINKIND_ATOM |
					ID::SUBKIND_ATOM_ORDINARYG);
			predAtom.tuple.push_back(idpred);
			OrdinaryAtom negpredAtom(
					ID::MAINKIND_ATOM |
					ID::SUBKIND_ATOM_ORDINARYG |
					ID::PROPERTY_AUX);
			negpredAtom.tuple.push_back(idnegpred);
			idatom = reg->storeOrdinaryGAtom(predAtom);
			idnegatom = reg->storeOrdinaryGAtom(negpredAtom);
		}
		else
		{
			// nonground atoms
			OrdinaryAtom predAtom(
					ID::MAINKIND_ATOM |
					ID::SUBKIND_ATOM_ORDINARYN);
			predAtom.tuple.push_back(idpred);
			OrdinaryAtom negpredAtom(
					ID::MAINKIND_ATOM |
					ID::SUBKIND_ATOM_ORDINARYN |
					ID::PROPERTY_AUX);
			negpredAtom.tuple.push_back(idnegpred);

			// add variables
			for(unsigned i = 0; i < arity; ++i)
			{
				// create variable
				std::ostringstream s;
				s << "X" << i;
				Term var(
						ID::MAINKIND_TERM |
						ID::SUBKIND_TERM_VARIABLE |
						ID::PROPERTY_AUX,
						s.str());
				const ID idvar = reg->storeConstOrVarTerm(var);
				predAtom.tuple.push_back(idvar);
				negpredAtom.tuple.push_back(idvar);
			}

			DBGLOG(DBG,"storing auxiliary atom " << predAtom);
			idatom = reg->storeOrdinaryNAtom(predAtom);
			DBGLOG(DBG,"storing auxiliary negative atom " << negpredAtom);
			idnegatom = reg->storeOrdinaryNAtom(negpredAtom);
		}

		// create constraint
		Rule r(
				ID::MAINKIND_RULE |
				ID::SUBKIND_RULE_CONSTRAINT |
				ID::PROPERTY_AUX);

		r.body.push_back(ID::posLiteralFromAtom(idatom));
		r.body.push_back(ID::posLiteralFromAtom(idnegatom));

		ID idcon = reg->storeRule(r);
		ctx.idb.push_back(idcon);
		DBGLOG(DBG,"created aux constraint '" <<
				printToString<RawPrinter>(idcon, reg) << "'");
	}

}

} // anonymous namespace

// rewrite program by adding auxiliary query rules
PluginRewriterPtr ActionPlugin::createRewriter(ProgramCtx& ctx)
{
	ActionPlugin::CtxData& ctxdata = ctx.getPluginData<ActionPlugin>();
	if( !ctxdata.enabled )
		return PluginRewriterPtr();

	return PluginRewriterPtr(new ActionPluginConstraintAdder);
}

namespace
{

class NegAuxPrinter:
	public AuxPrinter
{
public:
	typedef ActionPlugin::CtxData::NegToPosMap NegToPosMap;
public:
	NegAuxPrinter(
			RegistryPtr reg,
			PredicateMask& negAuxMask,
			const NegToPosMap& ntpm):
		reg(reg), mask(negAuxMask), ntpm(ntpm)
	{
	}

  // print an ID and return true,
  // or do not print it and return false
  virtual bool print(std::ostream& out, ID id, const std::string& prefix) const
	{
		assert(id.isAuxiliary());
		mask.updateMask();
		DBGLOG(DBG,"mask is " << *mask.mask());
		if( mask.mask()->getFact(id.address) )
		{
			// we cannot use any stored text to print this, we have to assemble it from pieces
			DBGLOG(DBG,"printing auxiliary for strong negation: " << id);

			// get replacement atom details
			const OrdinaryAtom& r_atom = reg->ogatoms.getByAddress(id.address);

			// find positive version of predicate
			assert(!r_atom.tuple.empty());
			const NegToPosMap::const_iterator itpred = ntpm.find(r_atom.tuple.front());
			assert(itpred != ntpm.end());
			const ID idpred = itpred->second;

			// print strong negation
			out << prefix << '-';

			// print tuple
      RawPrinter printer(out, reg);
      // predicate
      printer.print(idpred);
      if( r_atom.tuple.size() > 1 )
      {
        Tuple t(r_atom.tuple.begin()+1, r_atom.tuple.end());
        out << "(";
        printer.printmany(t,",");
        out << ")";
      }

			return true;
		}
		return false;
	}

protected:
	RegistryPtr reg;
	PredicateMask& mask;
	const NegToPosMap& ntpm;
};

} // anonymous namespace

// register auxiliary printer for strong negation auxiliaries
void ActionPlugin::setupProgramCtx(ProgramCtx& ctx)
{
	ActionPlugin::CtxData& ctxdata = ctx.getPluginData<ActionPlugin>();
	if( !ctxdata.enabled )
		return;

	RegistryPtr reg = ctx.registry();

	// init predicate mask
	ctxdata.myAuxiliaryPredicateMask.setRegistry(reg);

	// add all auxiliaries to mask (here we should already have parsed all of them)
	typedef CtxData::NegToPosMap NegToPosMap;
	NegToPosMap::const_iterator it;
	for(it = ctxdata.negToPos.begin();
			it != ctxdata.negToPos.end(); ++it)
	{
		ctxdata.myAuxiliaryPredicateMask.addPredicate(it->first);
	}

	// update predicate mask
	ctxdata.myAuxiliaryPredicateMask.updateMask();

	// create auxiliary printer using mask
	AuxPrinterPtr negAuxPrinter(new NegAuxPrinter(
				reg, ctxdata.myAuxiliaryPredicateMask, ctxdata.negToPos));
	reg->registerUserAuxPrinter(negAuxPrinter);
}

DLVHEX_NAMESPACE_END

// this would be the code to use this plugin as a "real" plugin in a .so file
// but we directly use it in dlvhex.cpp
#if 0
ActionPlugin theActionPlugin;

// return plain C type s.t. all compilers and linkers will like this code
extern "C"
void * PLUGINIMPORTFUNCTION()
{
	return reinterpret_cast<void*>(& DLVHEX_NAMESPACE theActionPlugin);
}

#endif
/* vim: set noet sw=2 ts=2 tw=80: */

// Local Variables:
// mode: C++
// End:
