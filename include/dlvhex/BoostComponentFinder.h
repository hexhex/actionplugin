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
 * @file BoostComponentFinder.h
 * @author Roman Schindlauer
 * @date Wed Jan 25 14:31:33 CET 2006
 *
 * @brief Strategy class for finding SCCs and WCCs from a given program graph, using
 * the Boost Graph Library
 *
 *
 */


#if !defined(_DLVHEX_BOOSTCOMPONENTFINDER_H)
#define _DLVHEX_BOOSTCOMPONENTFINDER_H

#include "dlvhex/PlatformDefinitions.h"

#include "dlvhex/ComponentFinder.h"

#include <vector>


DLVHEX_NAMESPACE_BEGIN

/**
 * @brief Component Finder using the Boost Graph Library.
 *
 */
class DLVHEX_EXPORT BoostComponentFinder : public ComponentFinder
{
public:

    /// Ctor.
    BoostComponentFinder()
    { }

    /**
     * @brief Finds Weakly Connected Components from a list of AtomNodes.
     *
     * A single WCC is represented by a vector of AtomNodes. All WCCs are again
     * contained in a vector.
     */
    virtual void
    findWeakComponents(const std::vector<AtomNodePtr>&,
                       std::vector<std::vector<AtomNodePtr> >&);

    /**
     * @brief Finds Strongly Connected Components from a list of AtomNodes.
     */
    virtual void
    findStrongComponents(const std::vector<AtomNodePtr>&,
                         std::vector<std::vector<AtomNodePtr> >&);
    
private:

    /**
     * @brief Converts the AtomNode dependency structure into Edges.
     *
     * The Boost Graph Library works with integers as vertex-identifier and
     * pairs of integers as edges. This function converts the dependency
     * information of the given AtomNodes and convets it into the Edges type.
     */
    void
    makeEdges(const std::vector<AtomNodePtr>&, Edges&) const;

    /**
     * @brief Filters a set of AtomNodes based on given Vertices.
     *
     * The Boost functions return sets of vertices as result. This function
     * selects those AtomNodes from a given set that correspond to these
     * Vertices. The correspondence between AtomNodes and Vertices is based on
     * the unique ID each AtomNode has.
     */
    void
    selectNodes(const Vertices&,
                const std::vector<AtomNodePtr>&,
                std::vector<AtomNodePtr>&) const;
};


DLVHEX_NAMESPACE_END

#endif /* _DLVHEX_BOOSTCOMPONENTFINDER_H_ */


// Local Variables:
// mode: C++
// End:
