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
 * @file OrdinaryModelGenerator.cpp
 * @author Roman Schindlauer
 * @date Tue Sep 13 18:45:17 CEST 2005
 *
 * @brief Strategy class for computing the model of a subprogram without
 * external atoms.
 *
 *
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "dlvhex/ModelGenerator.h"
#include "dlvhex/ASPSolver.h"
#include "dlvhex/globals.h"
#include "dlvhex/ProgramCtx.h"

DLVHEX_NAMESPACE_BEGIN

OrdinaryModelGenerator::OrdinaryModelGenerator(const ProgramCtx& c)
  : ModelGenerator(c)
{ }


void
OrdinaryModelGenerator::compute(const std::vector<AtomNodePtr>& nodes,
                                const AtomSet &I,
                                std::vector<AtomSet> &models)
{
  Program program;

  //
  // go through all nodes
  //
  std::vector<AtomNodePtr>::const_iterator node = nodes.begin();
  while (node != nodes.end())
    {
      const std::vector<Rule*>& rules = (*node)->getRules();
      
      //
      // add all rules from this node to the component
      //
      for (std::vector<Rule*>::const_iterator ri = rules.begin();
	   ri != rules.end();
	   ++ri)
	  {
            program.addRule(*ri);
	  }
      
      ++node;
    }
  
  this->compute(program, I, models);
}


void
OrdinaryModelGenerator::compute(const Program& program,
                                const AtomSet &I,
                                std::vector<AtomSet> &models)
{
  DEBUG_START_TIMER;

  models.clear();

  ///
  /// @todo: we don't get any extatom-replacement predicates in the
  /// result - the asp solver result parser would throw them away (ho)
  /// and so we couldn't get rid of them any more. this is why we have
  /// to add the input edb below again!
  ///
  std::auto_ptr<BaseASPSolver> solver(ctx.getProcess()->createSolver());

  std::vector<AtomSet> answersets;

  try
    {
      solver->solve(program, I, answersets);
    }
  catch (GeneralError&)
    {
      throw;
    }

  //@todo can we do better?
  for (std::vector<AtomSet>::iterator as = answersets.begin();
       as != answersets.end();
       ++as)
    {
      as->insert(I);
      models.push_back(*as);
    }

  //                123456789-123456789-123456789-123456789-123456789-123456789-123456789-123456789-
  DEBUG_STOP_TIMER("Ordinary Model Generator:               ");
}

DLVHEX_NAMESPACE_END

// Local Variables:
// mode: C++
// End:
