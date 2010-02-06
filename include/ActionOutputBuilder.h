/* dlvhex -- Answer-Set Programming with external interfaces.
 * Copyright (C) 
 * 
 * This file is part of dlvhex-dlplugin.
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
 * @file   ActionOutputBuilder.h
 * @author Selen Basol, Ozan Erdem
 * @date   Fri Dec 12 16:20:01 CET 2009
 * 
 * @brief  Builders for default rules output.
 * 
 * 
 */

#ifndef _DLVHEX_Action_OUTPUT_BUILDER_H_
#define _DLVHEX_Action_OUTPUT_BUILDER_H_

#include <iostream>
#include <vector>
#include "dlvhex/OutputBuilder.h"
#include "dlvhex/PrintVisitor.h"
#include "dlvhex/ResultContainer.h"



namespace dlvhex {

class ResultContainer;

namespace aa {


  class DLVHEX_EXPORT ActionOutputBuilder : public dlvhex::OutputBuilder
    {
    public:
      virtual
	~ActionOutputBuilder();

      ActionOutputBuilder();
	
     std::vector<AtomPtr>
     findIntersection(const std::vector<AnswerSet>& rc);

      virtual void
	buildResult(std::ostream&, const dlvhex::ResultContainer&);
    private:
	std::vector<AnswerSet> filtered_rc;
	std::vector<AtomPtr>* rc_intersection;
	std::vector<AtomPtr>* best_intersection;
    };

} // namespace aa
} // namespace dlvhex

#endif /* _DLVHEX_Action_OUTPUT_BUILDER_H_ */
