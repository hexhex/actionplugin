/* dlvhex-KBModaddon -- KBMod addon for dlvhex.
 * 2009 Selen Başol, Ozan Erdem
 *
 * This file is part of dlvhex-KBModaddon.
 *
 * dlvhex-KBModaddon is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * dlvhex-KBModaddon is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with dlvhex; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA.
 */

/**
 * @file   ExecuteAtom.cpp
 * @author Selen Başol
 * @author Ozan Erdem
 *
 * @brief Handling of execute atoms
 *
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "KBModAddon.h"
#include "ExecuteAtom.h"

#include <iostream>
#include <string>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>


namespace dlvhex
{
  namespace kbmod
  {


    ExecuteAtom::~ExecuteAtom()
    {
      
    }

	ExecuteAtom::ExecuteAtom(KBModAddon& kbmod) : kbmod_addon(kbmod)
	{
		//addInputConstant();	
		//addInputConstant();			
	}



    // Standard function takes the query as an input and evaluates the associated function 
    void
    ExecuteAtom::execute(const Tuple & parms) throw(PluginError)
    {

	  std::string command=parms[0].getString();
	  if(command[0]!='\"' || command[command.size()-1]!='\"')
	  {
	  		throw PluginError("The parameter of the atom #execute should be a quoted string!");
      }

	  
	  

	  command=command.substr(1,command.size()-2);
      system(command.c_str());

    }

    



 


  } // robot

} // dlvhex
