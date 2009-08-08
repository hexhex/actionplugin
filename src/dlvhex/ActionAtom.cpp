/* dlvhex -- Answer-Set Programming with action interfaces.
 * 2009 Selen Başol, Ozan Erdem
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
 * @file   ActionAtom.cpp
 * @author Selen Başol
 * @author Ozan Erdem
 * @date   Sat Aug 1 15:31:17 CEST 2009
 * 
 * @brief  Action class atom.
 * 
 * 
 */

#include "dlvhex/PluginContainer.h"
#include "dlvhex/PluginInterface.h"
#include "dlvhex/ActionAtom.h"
#include "dlvhex/Error.h"
#include "dlvhex/Registry.h"
#include "dlvhex/BaseVisitor.h"

#include <cassert>
#include <string>
#include <functional>

DLVHEX_NAMESPACE_BEGIN

ActionAtom::ActionAtom()
{ }


ActionAtom::ActionAtom(const ActionAtom& actatom)
  : Atom(),
    inputList(actatom.inputList),
    functionName(actatom.functionName),
    auxPredicate(actatom.auxPredicate),
    replacementName(actatom.replacementName),
    filename(actatom.filename),
    option(actatom.option),
    precedence(actatom.precedence),
    line(actatom.line)
{ }


ActionAtom::ActionAtom(const std::string& name,
                 const std::string& opt,
				 const Term& prec,
                 const Tuple& input,
                 const unsigned int line)
  : Atom(name),
    inputList(input),
    functionName(name),
    option(opt),
    precedence(prec),
    line(line)
	
{
  // setup replacement name and so forth
  initReplAux();
}


void
ActionAtom::initReplAux()
{
  ///@todo if the user names a predicate ACTATOM, we clash with
  ///these auxiliary names here

  //
  // make replacement name
  //
  replacementName = "__ACTION__" + functionName;

  //
  // remember this artificial atom name, we need to remove those
  // later, they shouldn't be in the actual result
  //
  Term::registerAuxiliaryName(replacementName);

  //
  // build auxiliary predicate
  //
  auxPredicate.clear();

  if (!this->pureGroundInput() && !this->isGroundPrecedence())
    {
      ///@todo this simple aux name does not work for nonground input!
      ///Maybe we can build a  map similar to the dl-atom rewriting in
      ///GraphBuilder?

      //
      // also produce auxiliary predicate name, we need this for the
      // nonground input list
      //
      auxPredicate = replacementName + "_aux";
    }
}



const std::string&
ActionAtom::getAuxPredicate() const
{
  return auxPredicate;
}


void
ActionAtom::setAuxPredicate(const std::string& auxname)
{
  auxPredicate = auxname;
}


void
ActionAtom::setFunctionName(const std::string& name)
{
  // set new functionName
  this->functionName = name;

  // and set predicate name? at least we need them in operator== and
  // unifiesWith
  this->arguments[0] = Term(name);

  // and now setup the new replacement and aux names
  initReplAux();
}


const std::string&
ActionAtom::getFunctionName() const
{
  return functionName;
}


const std::string&
ActionAtom::getReplacementName() const
{
  return replacementName;
}


bool
ActionAtom::pureGroundInput() const
{
  // if we cannot find a variable input argument, we are ground
  return (inputList.end() == std::find_if(inputList.begin(), inputList.end(), std::mem_fun_ref(&Term::isVariable)));
}

bool ActionAtom::isGroundPrecedence() const
{
	return !precedence.isVariable();
}



const Tuple&
ActionAtom::getInputTerms() const
{
  return inputList;
}


void
ActionAtom::setInputTerms(const Tuple& ninput)
{
  inputList.clear();
	
  // copy nargs to the 2nd position in inputList
  inputList.insert(inputList.end(), 
		   ninput.begin(),
		   ninput.end()
		   );

  ///@todo new replacementname for nonground input?
  ///@todo check parameter length?
}


bool
ActionAtom::unifiesWith(const AtomPtr& atom2) const
{
  //
  // action atoms only unify with action atoms of the the same
  // name, and the same arity of input and output lists
  //
  if (typeid(*(atom2.get())) == typeid(ActionAtom))
    {
      const ActionAtom* ea2 = static_cast<const ActionAtom*>(atom2.get());

      if (getFunctionName() == ea2->getFunctionName() &&
	  getArity() == ea2->getArity() &&
	  inputList.size() == ea2->inputList.size())
	{
	  // both input and output lists must unify
	  return
	  /*  std::equal(arguments.begin(), arguments.end(),
		       ea2->arguments.begin(),
		       std::mem_fun_ref(&Term::unifiesWith))
	    
	   */ option==ea2->option
	    &&
	    std::equal(inputList.begin(), inputList.end(),
		       ea2->inputList.begin(),
		       std::mem_fun_ref(&Term::unifiesWith));
	}
    }

  return false;
}


bool
ActionAtom::operator== (const Atom& atom2) const
{

  // don't forget, typeid gives the most derived type only if it is
  // applied to a non-pointer
  if (typeid(atom2) == typeid(ActionAtom))
    {
      const ActionAtom* aa2 = static_cast<const ActionAtom*>(&atom2);
      return isEqualSyntax(*aa2);

    }

  return false;
	
}

bool
ActionAtom::isEqualSyntax (const ActionAtom &atom2) const
{
	if( getOption()!=atom2.getOption() ||
		getFunctionName()!=atom2.getFunctionName() ||
		getPrecedence()!=atom2.getPrecedence() ||
		getInputTerms()!=atom2.getInputTerms() )
	{
		return false;
	}
	return true;
}



bool
ActionAtom::operator< (const Atom& atom2) const
{
	return true;
}



void
ActionAtom::accept(BaseVisitor& v)
{
  v.visit(this);
}

unsigned
ActionAtom::getLine() const
{
  return line;
}

const std::string&
ActionAtom::getOption() const
{
	return option;
}

const Term&
ActionAtom::getPrecedence() const
{
	return precedence;
}



DLVHEX_NAMESPACE_END

/* vim: set noet sw=2 ts=8 tw=80: */

// Local Variables:
// mode: C++
// End:
