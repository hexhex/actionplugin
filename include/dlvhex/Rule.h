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
 * @file Rule.h
 * @author Roman Schindlauer
 * @date Thu Jun 30 12:39:40 2005
 *
 * @brief Rule class.
 *
 */


#if !defined(_DLVHEX_RULE_H)
#define _DLVHEX_RULE_H

#include "dlvhex/PlatformDefinitions.h"

#include "dlvhex/Atom.h"
#include "dlvhex/Literal.h"

#include <boost/ptr_container/indirect_fun.hpp>


DLVHEX_NAMESPACE_BEGIN

/**
 * A rule head is a disjunction of Atoms.
 *
 * Since the Atoms in a rule's head are unordered, we use a std::set for them.
 *
 * \sa http://www.sgi.com/tech/stl/set.html
 */
typedef std::set<AtomPtr, boost::indirect_fun<std::less<Atom> > > RuleHead_t;


/**
 * @brief Class for representing a rule object.
 */
class DLVHEX_EXPORT Rule : public ProgramObject
{
public:

	/**
	 * @brief Constructs a rule from a head and a body.
	 *
	 * Third argument is the file name and fourth the line number this rule
	 * appeared in. Both can be ommitted.
	 */
	Rule(const RuleHead_t& h,
	     const RuleBody_t& b,
	     const std::string& = "",
	     unsigned = 0);

	/**
	 * Destructor.
	 */
	virtual
	~Rule();

	/**
	 * @brief Returns the rule's head.
	 */
	virtual const RuleHead_t&
	getHead() const;

	/**
	 * @brief Returns the rule's body.
	 */
	virtual const RuleBody_t&
	getBody() const;

	/**
	 * @brief Replaces the rule's head by the specified one.
	 */
	virtual void
	setHead(const RuleHead_t&);

	/**
	 * @brief Replaces the rule's body by the specified one.
	 */
	virtual void
	setBody(const RuleBody_t&);

	/**
	 * @brief add to rule's head.
	 */
	virtual void
	addHead(AtomPtr);

	/**
	 * @brief add to rule's body.
	 */
	virtual void
	addBody(Literal*);

	/**
	 * @brief Returns the filename where this rule appeared.
	 */
	const std::string&
	getFile() const;

	/**
	 * @brief Returns the program line number of this rule.
	 */
	unsigned
	getLine() const;

	/**
	 * @brief returns the rule's external atoms.
	 *
	 * In addition to head and body, a Rule also maintains a list of the
	 * external atoms in its body, which is easier to access than having to
	 * iterate through the entire body.
	 */
	const std::vector<ExternalAtom*>&
	getExternalAtoms() const;

	/**
	 * @brief Test for equality.
	 *
	 * Two rules are equal, if they contain the same atoms in the body and the head.
	 */
	bool
	operator== (const Rule& rule2) const;

	/**
	 * Less-than comparison.
	 *
	 * A rule is smaller than another, if the head of the first is smaller than
	 * the other rule's head. If both heads are equal, the same applies to their
	 * bodies. This operator doesn't make much sense on its own, but is needed
	 * for having a set-container of rules. (std::set requires less-than for its
	 * objects).
	 *
	 * \sa RuleHead_t::operator<, RuleBody_t::operator<
	 */
	virtual bool
	operator< (const Rule& rule2) const;

	/**
	 * @brief accepts a visitor.
	 *
	 * According to the visitor pattern, accept simply calls the respective
	 * visitor with the Rule itself as parameter.
	 *
	 * \sa http://en.wikipedia.org/wiki/Visitor_pattern
	 */
	virtual void
	accept(BaseVisitor&);

protected:

	/**
	 * Rule head.
	 */
	mutable RuleHead_t head;

	/**
	 * Rule body.
	 */
	RuleBody_t body;

	/**
	 * Filename where the rule was parsed from.
	 *
	 * Needed for error messages.
	 */
	std::string programFile;

	/**
	 * Line in which the rule appeared.
	 *
	 * Needed for error messages.
	 */
	unsigned programLine;

private:

	/**
	 * List of external atoms occuring in the rule's body.
	 */
	std::vector<ExternalAtom*> externalAtoms;

        /// non-virtual function for the ctor
        void
	setExternalAtoms(const RuleBody_t&);
};


/**
 * Direct serialization of a Rule using a RawPrintVisitor.
 *
 * Should be used for debugging or verbose only.
 */
std::ostream&
operator<< (std::ostream& out, const Rule& rule);

/**
 * Comparison operator for rule heads.
 *
 * Used for comparing rules.
 */
bool
operator< (const RuleHead_t& head1, const RuleHead_t& head2);



/**
 * @brief A weak constraint is a rule with empty head and weight/level values.
 */
class DLVHEX_EXPORT WeakConstraint : public Rule
{
public:

	/**
	 * @brief See constructor of Rule.
	 *
	 * The third parameter is the weight, the fourth is the level of the weak
	 * constraint.
	 */
	WeakConstraint(const RuleBody_t& b,
	               const Term&,
	               const Term&,
	               const std::string& = "",
	               unsigned = 0);

	/**
	 * @brief Test for equality.
	 */
	bool
	operator== (const WeakConstraint&) const;

	/**
	 * @brief accepts a visitor.
	 *
	 * According to the visitor pattern, accept simply calls the respective
	 * visitor with the weak constraint itself as parameter.
	 *
	 * \sa http://en.wikipedia.org/wiki/Visitor_pattern
	 */
	virtual void
	accept(BaseVisitor&);

	/**
	 * Returns the weight of the WC.
	 */
	const Term&
	getWeight() const
	{
		return weight;
	}

	/**
	 * Returns the level of the WC.
	 */
	const Term&
	getLevel() const
	{
		return level;
	}


	/**
	 * @brief build wc head.
	 */
	const RuleHead_t&
	getHead() const;

	/**
	 * @brief set new wc body and clear head.
	 */
	void
	setBody(const RuleBody_t&);

	/**
	 * @brief add to wc's body and clear head.
	 */
	virtual void
	addBody(Literal*);


private:
	/**
	 * @brief disallow setHead.
	 */
	void
	setHead(const RuleHead_t&);

	/**
	 * @brief disallow addHead.
	 */
	void
	addHead(AtomPtr);

        /// wc weight
        Term weight;
        /// wc level
        Term level;
        /// every wc has an unique identifier
        unsigned uniqueID;
};

DLVHEX_NAMESPACE_END

#endif /* _DLVHEX_RULE_H */

/* vim: set noet sw=4 ts=4 tw=80: */


// Local Variables:
// mode: C++
// End:
