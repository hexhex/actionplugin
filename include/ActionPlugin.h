/* dlvhex -- Answer-Set Programming with external interfaces.
 * Copyright (C) 2005, 2006, 2007 Roman Schindlauer
 * Copyright (C) 2006, 2007, 2008, 2009, 2010, 2011 Thomas Krennwallner
 * Copyright (C) 2009, 2010, 2011 Peter Schüller
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
 * @file ActionPlugin.h
 * @author Stefano Germano
 *
 * @brief Plugin ...
 */

#ifndef ACTION_PLUGIN_H
#define ACTION_PLUGIN_H

#include "dlvhex2/PlatformDefinitions.h"
#include "dlvhex2/PluginInterface.h"

#include "Action.h"

DLVHEX_NAMESPACE_BEGIN

class ActionPlugin: public PluginInterface {
  public:
    // stored in ProgramCtx, accessed using getPluginData<ActionPlugin>()
    class CtxData: public PluginData {
      public:
        // whether plugin is enabled
        bool enabled;

//    // predicate constants which were encountered in negative form and their arity
//    typedef std::map<ID,unsigned> PredicateArityMap;
//    PredicateArityMap negPredicateArities;
//
//    // aux predicate constants and their positive counterparts
//    typedef std::map<ID,ID> NegToPosMap;
//    NegToPosMap negToPos;

// for fast detection whether an ID is this plugin's responsitility to display
        PredicateMask myAuxiliaryPredicateMask;

        // an id that is stored in Registry and give the string representing the name of each action atom "rewritten"
        dlvhex::ID id_in_the_registry;

        dlvhex::ID id_brave;
        dlvhex::ID id_cautious;
        dlvhex::ID id_preferred_cautious;

        dlvhex::ID id_default_priority;
        dlvhex::ID id_default_weight;
        dlvhex::ID id_default_level;

        typedef std::map<ID, Action> IDActionMap;
        IDActionMap idActionMap;

        CtxData();
        virtual ~CtxData() {
        }
        ;
    };

  public:
    ActionPlugin();
    virtual ~ActionPlugin();

    // output help message for this plugin
    virtual void printUsage(std::ostream& o) const;

    // accepted options: --strongnegation-enable
    //
    // processes options for this plugin, and removes recognized options from pluginOptions
    // (do not free the pointers, the const char* directly come from argv)
    virtual void processOptions(std::list<const char*>& pluginOptions, ProgramCtx&);

    // create parser modules that extend and the basic hex grammar
    virtual std::vector<HexParserModulePtr> createParserModules(ProgramCtx&);

//  // rewrite program by adding auxiliary constraints
//  virtual PluginRewriterPtr createRewriter(ProgramCtx&);

    // change model callback (print auxiliaries as negative atoms)
    virtual void setupProgramCtx(ProgramCtx&);

    // no atoms!
};

DLVHEX_NAMESPACE_END

#endif
