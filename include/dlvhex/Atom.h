/* dlvhex -- Answer-Set Programming with external interfaces.
 * Copyright (C) 2005, 2006, 2007 Roman Schindlauer
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
 * @file Atom.h
 * @author Roman Schindlauer
 * @date Mon Sep  5 16:57:17 CEST 2005
 *
 * @brief Atom base class.
 *
 *
 */

#if !defined(_DLVHEX_ATOM_H)
#define _DLVHEX_ATOM_H

#include "dlvhex/PlatformDefinitions.h"

#include "dlvhex/Term.h"
#include "dlvhex/Repository.h"

#include <list>
#include <set>

#include <boost/shared_ptr.hpp>


DLVHEX_NAMESPACE_BEGIN


class Atom;

/**
 * Atom Pointer.
 *
 * \ingroup dlvhextypes
 *
 * We use the shared pointer type from the boost-library instead of Atom* to
 * handle pointers to atoms. These shared pointers maintain a reference count
 * and automatically delete the object when the last reference disappears. This
 * is convenient in our framework, since The same atom object might be
 * referenced in many different modules.
 *
 * Instead of creating an Atom in the classical way,
 *
 * \code
 * Atom* a = new Atom("foo");
 * \endcode
 *
 * we use this:
 *
 * \code
 * AtomPtr a(new Atom("foo"));
 * \endcode
 *
 * or this:
 *
 * \code
 * AtomPtr a;
 * ...
 * a = AtomPtr(new Atom("foo"));
 * \endcode
 */
typedef boost::shared_ptr<Atom> AtomPtr;

class BuiltinPredicate;
typedef boost::shared_ptr<BuiltinPredicate> BuiltinPredicatePtr;


/**
 * @brief An Atom has a predicate and (if not propositional) an optional list of arguments.
 *
 * \ingroup dlvhextypes
 *
 * An Atom corresponds to a logical atom.
 *
 */
class DLVHEX_EXPORT Atom : public ProgramObject
{
protected:
  /**
   * Default constructor is protected, we do not want to create void atoms.
   */
  Atom();

public:

	/**
	 * Destructor.
	 */
	virtual
	~Atom();

	/**
	 * Copy constructor.
	 */
	Atom(const Atom&);

	/**
	 * Constructs an atom from a string.
	 *
	 * This can be:
	 * - A propositional atom, like \b lightOn
	 * - A first order atom, like \b p(X) or \b q(a,b,Z)
	 * .
	 * If the atom is propositional, it must be ground, i.e., the string must
	 * not denote a single variable.
	 * The second argument indicates whether the atom is strongly negated.
	 */
	explicit
	Atom(const std::string&, bool = false);

	/**
	 * Constructs an atom from a predicate string and a tuple.
	 * The string denotes the predicate symbol of the atom, the tuple its
	 * arguments.
	 * The third argument indicates if the atom is strongly negated.
	 * The tuple can also be empty, then the atom is propositional and consists
	 * only of the predicate identifier (which must not be a variable then).
	 */
	Atom(const std::string&, const Tuple&, bool = false);

	/**
	 * Constructs an atom from a list of arguments.
	 *
	 * This is another way of constructing an atom, reflecting the notion of
	 * higher-order syntax, where the predicate is just a Term inside the
	 * argument list.  The first element of the tuple is considered to be the
	 * predicate - this is important for the usage of Atom::getPredicate() and
	 * Atom::getArguments().  The second argument indicates if the atom is strongly
	 * negated. The tuple must not be empty or contain a single variable term.
	 */
	explicit
	Atom(const Tuple&, bool = false);

	/**
	 * Returns the predicate of the atom.
	 *
	 * If the atom was constructed as a propositional atom, then the entire atom
	 * is returned.
	 */
	virtual const Term&
	getPredicate() const;

	/**
	 * Returns the arguments of an atom.
	 *
	 * If the atom is propositional, an empty Tuple is returned.
	 */
	virtual inline Tuple
	getArguments() const
	{
		assert(!arguments.empty());
		return Tuple(++arguments.begin(), arguments.end());
	}

   /**
	 * @brief sets arguments of an atom.
	 */
    virtual void
	setArguments(const Tuple& nargs);

	/**
	 * @brief Returns the specified argument term.
	 *
	 * The arguments of an n-ary atom are numbered from 1 to n. An index of 0
	 * returns the predicate symbol of the atom.
	 */
	const Term&
	getArgument(unsigned index) const;

	/**
	 * @brief Sets argument at index position of an atom.
	 */
	void setArgument(unsigned index, const Term& term2);
		
	/**
	 * @brief Sets first argument (corresponding to the predicate) of an atom.
	 */
	void setPredicate(const Term& term2);
	
	/**
	 * Returns the arity of an atom (number of arguments).
	 *
	 * For traditional atoms this works as expected:
	 * - \b p(q)  has arity 1
	 * - \b a	 has arity 0
	 * .
	 * For atoms that were constructed from tuple-syntax, the arity is one less
	 * than the original tuple's arity, since the first term of the tuple is
	 * regarded as the atom's predicate:
	 * - \b (X,Y) has arity 1 (seen as <b>X(Y)</b>)
	 * .
	 */
	virtual unsigned
	getArity() const;
	
	/**
	 * @brief Tests for unification with another atom.
	 *
	 * Two atoms unify if they have the same arity and all of their arguments
	 * (including the predicate symbols) unify pairwise.
	 *
	 * @see Term::unifiesWith()
	 */
	virtual bool
	unifiesWith(const AtomPtr&) const;

	/**
	 * @brief Tests for equality.
	 *
	 * Two Atoms are equal, if they have the same arity and list of arguments
	 * (including the predicate). Two variable arguments are equal in this context, if
	 * their strings are equal.
	 * Two atoms of different class (e.g., ExternalAtom and Atom) are always inequal.
	 */
	virtual bool
	operator== (const Atom& atom2) const;

	/**
	 * Tests for inequality.
	 *
	 * Negation of Atom::operator==.
	 */
	bool
	operator!= (const Atom& atom2) const;

	/**
	 * Comparison operator.
	 *
	 * A comparison operator is needed for collecting Atom-objects in a
	 * std::set. First, the predicates are compared via Term::compare(). If the
	 * result of the term-comparison is negative, 1 is returned, 0 otherwise.
	 * are equal, the arities are compared: smaller arity yields a "smaller"
	 * atom (Having different arities with the same predicate can happen for
	 * atoms with variable predicates!). If arities are equal as well, the
	 * atoms' arguments are compared, from left to right, via Term::compare().
	 */
	virtual bool
	operator< (const Atom& atom2) const;
	
	/**
	 * @brief Avoids serializing this Atom as a higher-order atom.
	 *
	 * @todo doc
	 */
	void
	setAlwaysFO();

	/**
	 * @todo doc
	 */
	bool
	getAlwaysFO() const;

	/**
	 * @brief Accepts a visitor.
	 *
	 * A visitor is a common design pattern to implement context-specific
	 * operations outside the class. We use visitors for serialization of
	 * objects. This function calls the visitAtom() method of the specified
	 * visitor, passing itself as parameter. The visitor serializes the atom
	 * according to its (the visitor's) type (plain text, XML, etc.)
	 */
	virtual void
	accept(BaseVisitor&);
	
	/**
	 * @brief Tests if the atom contains only constant arguments.
	 */
	bool
	isGround() const;

	/**
	 * Tests whether the atom was constructed as strongly negated.
	 */
	bool
	isStronglyNegated() const;

	/**
	 * @brief Switches (classical) truth value.
	 */
	inline void
	negate()
	{
		isStrongNegated = !isStrongNegated;
	}
	
protected:

	/**
	 * Arguments of the atom.
	 *
	 * The predicate is considered as an argument itself, being the first term
	 * in the tuple.
	 */
	Tuple arguments;

	/**
	 * Flag indicating whehter the atoms was constructed as strongly negated.
	 */
	bool isStrongNegated;

	/**
	 * @todo doc
	 */
	bool isAlwaysFO;
};


/**
 * This operator should be used only for debugging and verbosity purposes.
 *
 * It uses the first-order notation. Proper serialization of an Atom happens
 * through the PrintVisitor class.
 */
std::ostream&
operator<< (std::ostream&, const Atom&);


/**
 * @brief Special atom denoting either true or false.
 *
 * boolAtom does not unify with any other atom and can be used wherever 'true'
 * or 'false' are implicitly expected, e.g., as rule head for constraints.
 */
class DLVHEX_EXPORT boolAtom : public Atom
{
public:
	boolAtom() : Atom(Tuple(1, Term("")))
	{ }

	virtual const Term&
	getPredicate() const
	{
		return arguments.front();
	}
			
	virtual bool
	unifiesWith(const AtomPtr&) const
	{
		return false;
	}

	virtual bool
	operator== (const Atom&) const
	{
		return false;
	}

	bool
	operator< (const Atom&) const
	{
		return true;
	}
};


/**
 * @brief Builtin Atom.
 *
 * This class represents atoms for builtin-predicates of dlv. For now, we just
 * pass the string on to the ASP solver and do not process it in any other way.
 *
 * The terms of a builtin are stored as atom arguments, the operator as
 * predicate.
 */
class DLVHEX_EXPORT BuiltinPredicate : public Atom
{
public:
	// binary predicate
	BuiltinPredicate(const Term&, const Term&, const std::string&);
	// ternary predicate
	BuiltinPredicate(const Term&, const Term&, const Term&, const std::string&);

	/**
	 * @brief accepts a visitor.
	 */
	virtual void
	accept(BaseVisitor&);

};


DLVHEX_NAMESPACE_END

#endif /* _DLVHEX_ATOM_H */

/* vim: set noet sw=4 ts=4 tw=80: */


// Local Variables:
// mode: C++
// End:
