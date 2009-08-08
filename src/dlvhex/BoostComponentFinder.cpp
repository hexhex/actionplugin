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
 * @file BoostComponentFinder.cpp
 * @author Roman Schindlauer
 * @date Wed Jan 25 14:34:20 CET 2006
 *
 * @brief Strategy class for finding SCCs and WCCs from a given program graph, using
 * the Boost Graph Library.
 *
 *
 */

#include "dlvhex/BoostComponentFinder.h"
#include "dlvhex/globals.h"
#include "dlvhex/PrintVisitor.h"
#include "dlvhex/Component.h"

#include <sstream>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/strong_components.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/algorithm/string/replace.hpp>


DLVHEX_NAMESPACE_BEGIN

void
BoostComponentFinder::makeEdges(const std::vector<AtomNodePtr>& nodes,
                                Edges& edges) const
{
    for (std::vector<AtomNodePtr>::const_iterator node = nodes.begin();
         node != nodes.end();
         ++node)
    {
        ComponentFinder::Vertex v1 = (*node)->getId();

        //
        // considering all types of dependencies
        //
        for (std::set<Dependency>::const_iterator d = (*node)->getSucceeding().begin();
            d != (*node)->getSucceeding().end();
            ++d)
        {
            ComponentFinder::Vertex v2 = (*d).getAtomNode()->getId();

            //
            // making edge from v2 to v1, because the direction is not relevant
            // for finding WCCs and SCCs and this is the "traditional" direction
            // for lp-dependency (head->body), and we want these arrows in the
            // graphviz output (see below)!
            // 
            edges.push_back(ComponentFinder::Edge(v2, v1));
        }

    }
}


void
BoostComponentFinder::selectNodes(const Vertices& vertices,
                                  const std::vector<AtomNodePtr>& nodes,
                                  std::vector<AtomNodePtr>& newnodes) const
{
    newnodes.clear();
    
    for (ComponentFinder::Vertices::const_iterator vi = vertices.begin();
            vi != vertices.end();
            ++vi)
    {
        ///@todo this is too expensive - all the vertices-stuff should
        /// actually be done in boost, it could handle all these
        /// properties internally.  there shouldn't be any mapping and
        /// searching here!
        std::vector<AtomNodePtr>::const_iterator an;

        for (an = nodes.begin(); an != nodes.end(); ++an)
        {
            if ((*an)->getId() == *vi)
                break;
        }

        if (an != nodes.end())
        {
            newnodes.push_back(*an);
        }
    }
}


void
BoostComponentFinder::findWeakComponents(const std::vector<AtomNodePtr>& nodes,
                                         std::vector<std::vector<AtomNodePtr> >& wccs)
{
    /*
    std::map<int, Vertex> vmap;

    Edges contedges;

    for (Edges::const_iterator ei = edges.begin();
         ei != edges.end();
         ++ei)
    {
        
    }
    */

    ComponentFinder::Edges edges;

    makeEdges(nodes, edges);

    using namespace boost;
    {
        typedef adjacency_list <listS, vecS, undirectedS> Graph;

        Graph G;

        for (Edges::const_iterator ei = edges.begin();
             ei != edges.end();
             ++ei)
        {
            add_edge(ei->first, ei->second, G);
        }

        std::vector<int> component(num_vertices(G));

        int num = connected_components(G, &component[0]);

//        std::cout << "Total number of components: " << num << std::endl;

        std::vector<AtomNodePtr> wcc;

        for (int cn = 0; cn < num; ++cn)
        {
            Vertices thiscomponent;

            for (std::vector<int>::size_type i = 0; i != component.size(); ++i)
            {
                if (component[i] == cn)
                {
                    thiscomponent.push_back(Vertex(i));
                }
            }

            //
            // hack - remove all single noded-components, as long as we don't know
            // how to use boost with non-contiguous vertices!
            //
            if (thiscomponent.size() > 1)
            {
                //.push_back(thiscomponent);

                wcc.clear();
                
                selectNodes(thiscomponent, nodes, wcc);

                wccs.push_back(wcc);
            }
        }

//        for (std::vector<int>::size_type i = 0; i != component.size(); ++i)
//            std::cout << "Vertex " << i <<" is in component " << component[i] << std::endl;
//        std::cout << std::endl;
        
    }
}


void
BoostComponentFinder::findStrongComponents(const std::vector<AtomNodePtr>& nodes,
                                           std::vector<std::vector<AtomNodePtr> >& sccs)
{
    ComponentFinder::Edges edges;

    makeEdges(nodes, edges);

    using namespace boost;
    {
        typedef adjacency_list<vecS, vecS, directedS> Graph;

        Graph G(0);

        for (Edges::const_iterator ei = edges.begin();
             ei != edges.end();
             ++ei)
	  {
            add_edge(ei->first, ei->second, G);
	  }

        std::vector<int> component(num_vertices(G));

        int num = strong_components(G, &component[0]);
        //std::cout << "Total number of components: " << num << std::endl;

        std::vector<AtomNodePtr> scc;

        for (int cn = 0; cn < num; ++cn)
        {
            Vertices thiscomponent;

            for (std::vector<int>::size_type i = 0; i != component.size(); ++i)
            {
                if (component[i] == cn)
                    thiscomponent.push_back(Vertex(i));
            }

            /// @todo boost adds also single components as strong
            /// components. we avoid that for now (25-01-06), because
            /// having so many sccs will mean to call dlv a lot, which
            /// might cost a lot. better to put an effort into the
            /// graphprocessor strategy of finding big wccs on the
            /// fly.

            /// @todo try the other way by using a different
            /// boostcomponentfinder with a different graphprocessor!

            //
            // only add components with more than one vertex:
            //
            if (thiscomponent.size() > 1)
            {
                //components.push_back(thiscomponent);

                scc.clear();
                
                selectNodes(thiscomponent, nodes, scc);

                sccs.push_back(scc);
            }
        }

	if (Globals::Instance()->doVerbose(Globals::DUMP_DEPENDENCY_GRAPH))
	  {  
	    //
	    // create a label table for the graphviz output
	    //
	    std::string nms[3 * nodes.size()];

	    std::ostringstream oss;
	    RawPrintVisitor rpv(oss);

	    for (unsigned y = 0; y < nodes.size(); ++y)
	      {
		oss.str("");
		
		nodes[y]->getAtom()->accept(rpv);

		std::string at(oss.str());

		boost::algorithm::replace_all(at, "\"", "\\\"");

		nms[y] = at;
	      }

	    std::ofstream out;
			
	    out.open(Globals::Instance()->lpfilename.c_str());
	    write_graphviz(out, G, make_label_writer(nms));
	    out.close();
	    
	    Globals::Instance()->getVerboseStream() << "Graph written to "
						    << Globals::Instance()->lpfilename << std::endl;
	  }
    }
}

DLVHEX_NAMESPACE_END


// Local Variables:
// mode: C++
// End:
