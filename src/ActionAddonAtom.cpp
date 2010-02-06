/* dlvhex -- Answer-Set Programming with external interfaces.
 * 
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
 * @file   ActionAddonInterface.cpp
 * @author Selen Basol, Ozan Erdem
 * @date   Mon Oct 17 14:37:07 CEST 2009
 * 
 * @brief Definition of Classes ActionAddonAtom,
 * 
 * 
 *      
 */     

#include "ActionAddonAtom.h"

DLVHEX_NAMESPACE_BEGIN

ActionAddonAtom::Query::Query( const Tuple& in)
    : input(in)
{
}


const Tuple&
ActionAddonAtom::Query::getInputTuple() const
{
    return input;
}
        


void
ActionAddonAtom::addInputPredicate()
{
	// throw error if last input term was tuple
	if (inputType.size() > 0)
		if (inputType.back() == TUPLE)
			throw GeneralError("Tuple inputs must be specified last in input list");

    inputType.push_back(PREDICATE);
}


void
ActionAddonAtom::addInputConstant()
{
	// throw error if last input term was tuple
	if (inputType.size() > 0)
		if (inputType.back() == TUPLE)
			throw GeneralError("Tuple inputs must be specified last in input list");

    inputType.push_back(CONSTANT);
}

void
ActionAddonAtom::addConstant()
{
	// throw error if last input term was tuple
	if (inputType.size() > 0)
		if (inputType.back() == TUPLE)
			throw GeneralError("Tuple inputs must be specified last in input list");

    inputType.push_back(CONSTANT);
}


void
ActionAddonAtom::addInputTuple()
{
    inputType.push_back(TUPLE);
}


bool
ActionAddonAtom::checkInputArity(const unsigned arity) const
{
  bool ret = (inputType.size() == arity);

  if (!inputType.empty())
    {
      return inputType.back() == TUPLE ? true : ret;
    }
  else
    {
      return ret;
    }
}



const std::vector<ActionAddonAtom::InputType>&
ActionAddonAtom::getInputTypes() const
{
  return inputType;
}


ActionAddonAtom::InputType
ActionAddonAtom::getInputType(const unsigned index) const
{
	if (index >= inputType.size())
	{
		assert(inputType.back() == TUPLE);
		return inputType.back();
	}

    return inputType[index];
}



DLVHEX_NAMESPACE_END

// Local Variables:
// mode: C++
// End:
