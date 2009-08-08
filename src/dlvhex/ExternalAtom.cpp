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
 * @file   ExternalAtom.cpp
 * @author Roman Schindlauer
 * @date   Wed Sep 21 19:45:11 CEST 2005
 * 
 * @brief  External class atom.
 * 
 * 
 */

#include "dlvhex/PluginContainer.h"
#include "dlvhex/PluginInterface.h"
#include "dlvhex/ExternalAtom.h"
#include "dlvhex/Error.h"
#include "dlvhex/Registry.h"
#include "dlvhex/BaseVisitor.h"

#include <cassert>
#include <string>
#include <functional>

DLVHEX_NAMESPACE_BEGIN

ExternalAtom::ExternalAtom()
{ }


ExternalAtom::ExternalAtom(const ExternalAtom& extatom)
  : Atom(extatom.arguments),
    inputList(extatom.inputList),
    functionName(extatom.functionName),
    auxPredicate(extatom.auxPredicate),
    replacementName(extatom.replacementName),
    filename(extatom.filename),
    line(extatom.line)
{ }


ExternalAtom::ExternalAtom(const std::string& name,
			   const Tuple& params,
			   const Tuple& input,
			   const unsigned line)
  : Atom(name, params),
    inputList(input),
    functionName(name),
    line(line)
{
  // setup replacement name and so forth
  initReplAux();
}


void
ExternalAtom::initReplAux()
{
  ///@todo if the user names a predicate exEXTATOM, we clash with
  ///these auxiliary names here

  //
  // make replacement name
  //
  replacementName = "ex" + functionName;

  //
  // remember this artificial atom name, we need to remove those
  // later, they shouldn't be in the actual result
  //
  Term::registerAuxiliaryName(replacementName);

  //
  // build auxiliary predicate
  //
  auxPredicate.clear();

  if (!this->pureGroundInput())
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
ExternalAtom::getAuxPredicate() const
{
  return auxPredicate;
}


void
ExternalAtom::setAuxPredicate(const std::string& auxname)
{
  auxPredicate = auxname;
}


void
ExternalAtom::setFunctionName(const std::string& name)
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
ExternalAtom::getFunctionName() const
{
  return functionName;
}


const std::string&
ExternalAtom::getReplacementName() const
{
  return replacementName;
}


bool
ExternalAtom::pureGroundInput() const
{
  // if we cannot find a variable input argument, we are ground
  return inputList.end() == std::find_if(inputList.begin(), inputList.end(), std::mem_fun_ref(&Term::isVariable));
}


const Tuple&
ExternalAtom::getInputTerms() const
{
  return inputList;
}


void
ExternalAtom::setInputTerms(const Tuple& ninput)
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
ExternalAtom::unifiesWith(const AtomPtr& atom2) const
{
  //
  // external atoms only unify with external atoms of the the same
  // name, and the same arity of input and output lists
  //
  if (typeid(*(atom2.get())) == typeid(ExternalAtom))
    {
      const ExternalAtom* ea2 = static_cast<const ExternalAtom*>(atom2.get());

      if (getFunctionName() == ea2->getFunctionName() &&
	  getArity() == ea2->getArity() &&
	  inputList.size() == ea2->inputList.size())
	{
	  // both input and output lists must unify
	  return
	    std::equal(arguments.begin(), arguments.end(),
		       ea2->arguments.begin(),
		       std::mem_fun_ref(&Term::unifiesWith))
	    &&
	    std::equal(inputList.begin(), inputList.end(),
		       ea2->inputList.begin(),
		       std::mem_fun_ref(&Term::unifiesWith));
	}
    }

  return false;
}


bool
ExternalAtom::operator== (const Atom& atom2) const
{
  bool ret = false;

  // don't forget, typeid gives the most derived type only if it is
  // applied to a non-pointer
  if (typeid(atom2) == typeid(ExternalAtom))
    {
      const ExternalAtom* ea2 = static_cast<const ExternalAtom*>(&atom2);

      if (this->functionName == ea2->functionName &&
	  this->getArity() == ea2->getArity() &&
	  this->isStrongNegated == ea2->isStrongNegated &&
	  this->inputList == ea2->inputList)
	{
	  ret = std::equal(this->arguments.begin(), this->arguments.end(),
			   ea2->arguments.begin());
	}
    }

  return ret;
}


bool
ExternalAtom::operator< (const Atom& atom2) const
{
  bool ret = false;
  if (typeid(atom2) == typeid(ExternalAtom))
  {
    const ExternalAtom* ea2 = static_cast<const ExternalAtom*>(&atom2);

    // dummy loop so that we can break out of it
    do
    {
      if (this->functionName < ea2->functionName)
      {
	ret = true;
      }
      else if (this->functionName > ea2->functionName)
      {
	// must be false
	break;
      }

      if (this->getArity() < ea2->getArity())
      {
	ret = true;
      }
      else if (this->getArity() > ea2->getArity())
      {
	// must be false
	break;
      }

      if (this->isStrongNegated < ea2->isStrongNegated)
      {
	ret = true;
      }
      else if (this->isStrongNegated > ea2->isStrongNegated)
      {
	// must be false
	break;
      }

      if (this->inputList < ea2->inputList)
      {
	ret = true;
      }
      else if (this->inputList > ea2->inputList)
      {
	// must be false
	break;
      }

      if (this->getArguments() < ea2->getArguments())
      {
	ret = true;
      }
      else if (this->getArguments() > ea2->getArguments())
      {
	// must be false
	break;
      }
    }
    while(false);
  }

  return ret;
}


void
ExternalAtom::accept(BaseVisitor& v)
{
  v.visit(this);
}


unsigned
ExternalAtom::getLine() const
{
  return line;
}

DLVHEX_NAMESPACE_END

/* vim: set noet sw=2 ts=8 tw=80: */

// Local Variables:
// mode: C++
// End:
