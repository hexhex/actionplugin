/* dlvhex -- Answer-Set Programming with external interfaces.
 * Copyright (C) 2005, 2006, 2007 Roman Schindlauer
 * Copyright (C) 2007, 2008 Thomas Krennwallner
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
 * @file   State.h
 * @author Thomas Krennwallner
 * @date   
 * 
 * @brief  
 * 
 */

#if !defined(_DLVHEX_STATE_H)
#define _DLVHEX_STATE_H

#include "dlvhex/PlatformDefinitions.h"
#include "dlvhex/ActionAtom.h"
#include "dlvhex/Atom.h"
#include "dlvhex/ResultContainer.h"

#include <boost/shared_ptr.hpp>
#include <vector>

DLVHEX_NAMESPACE_BEGIN

// forward declarations
class ProgramCtx;

/**
 * @brief State base class.
 */
class DLVHEX_EXPORT State
{
 protected:
  void
  changeState(ProgramCtx*, const boost::shared_ptr<State>&);

 public:
  virtual
  ~State()
  {}

  virtual void
  openPlugins(ProgramCtx*);

  virtual void
  convert(ProgramCtx*);

  virtual void
  parse(ProgramCtx*);

  virtual void
  rewrite(ProgramCtx*);

  virtual void
  createNodeGraph(ProgramCtx*);

  virtual void
  optimize(ProgramCtx*);

  virtual void
  safetyCheck(ProgramCtx*);

  virtual void
  strongSafetyCheck(ProgramCtx*);

  virtual void
  createDependencyGraph(ProgramCtx*);

  virtual void
  setupProgramCtx(ProgramCtx*);

  virtual void
  evaluate(ProgramCtx*);

  virtual void
  postProcess(ProgramCtx*);

  virtual void
  output(ProgramCtx*);

  virtual void
  createSchema(ProgramCtx*);

  virtual void 
  evaluateActions(ProgramCtx*);

};


class DLVHEX_EXPORT OpenPluginsState : public State
{
 public:
  virtual void
  openPlugins(ProgramCtx*);
};


class DLVHEX_EXPORT ConvertState : public State
{
 public:
  virtual void
  convert(ProgramCtx*);
};


class DLVHEX_EXPORT ParseState : public State
{
 public:
  virtual void
  parse(ProgramCtx*);
};


class DLVHEX_EXPORT RewriteState : public State
{
 public:
  virtual void
  rewrite(ProgramCtx*);
};


class DLVHEX_EXPORT CreateNodeGraph : public State
{
 public:
  virtual void
  createNodeGraph(ProgramCtx*);
};


class DLVHEX_EXPORT OptimizeState : public State
{
 public:
  virtual void
  optimize(ProgramCtx*);
};


class DLVHEX_EXPORT CreateDependencyGraphState : public State
{
 public:
  virtual void
  createDependencyGraph(ProgramCtx*);
};


class DLVHEX_EXPORT SafetyCheckState : public State
{
 public:
  virtual void
  safetyCheck(ProgramCtx*);
};


class DLVHEX_EXPORT StrongSafetyCheckState : public State
{
 public:
  virtual void
  strongSafetyCheck(ProgramCtx*);
};


class DLVHEX_EXPORT SetupProgramCtxState : public State
{
 public:
  virtual void
  setupProgramCtx(ProgramCtx*);
};


class DLVHEX_EXPORT EvaluateDepGraphState : public State
{
 public:
  virtual void
  evaluate(ProgramCtx*);
};


class DLVHEX_EXPORT EvaluateProgramState : public State
{
 public:
  virtual void
  evaluate(ProgramCtx*);
};


class DLVHEX_EXPORT PostProcessState : public State
{
 public:
  virtual void
  postProcess(ProgramCtx*);
};


class DLVHEX_EXPORT OutputState : public State
{
 public:
  virtual void
  output(ProgramCtx*);
};

class DLVHEX_EXPORT ActionState : public State
{
 public:
  virtual void
  createSchema(ProgramCtx*);

  virtual void
  evaluateActions(ProgramCtx*);

  std::vector<ActionAtom*>
  findIntersection(ResultContainer*);

  struct DLVHEX_EXPORT PrecedenceCompare
  {
	bool
	operator() (ActionAtom* aa1,ActionAtom* aa2)
	{
		return (aa1->getPrecedence()).getInt() <= (aa2->getPrecedence()).getInt();
	}
  } comparatorObject;



 private:
  std::vector<ActionAtom*> schema;
  std::vector<ActionAtom*> sorted_best_intersection,sorted_full_intersection;
  AnswerSetPtr bestmodel;
  struct DLVHEX_EXPORT ActionAtomPtrCompare
  {
	bool
	operator() (const ActionAtom* aa1,const ActionAtom* aa2)
	{
		return !(aa1->isEqualSyntax(*aa2));
    }
  };

};




DLVHEX_NAMESPACE_END

#endif // _DLVHEX_STATE_H

// Local Variables:
// mode: C++
// End:
