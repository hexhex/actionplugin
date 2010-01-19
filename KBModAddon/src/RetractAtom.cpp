/* dlvhex-KBModplugin -- KBMod plugin for dlvhex.
 * 2009 Selen Başol, Ozan Erdem
 *
 * This file is part of dlvhex-KBModplugin.
 *
 * dlvhex-robotplugin is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * dlvhex-robotplugin is distributed in the hope that it will be
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
 * @file   RetractAtom.cpp
 * @author Selen Başol
 * @author Ozan Erdem
 *
 * @brief Handling of assert atoms
 *
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "KBModAddon.h"
#include "RetractAtom.h"

#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <sstream>

namespace dlvhex
{
  namespace kbmod
  {


    RetractAtom::~RetractAtom()
    {
      
    }

	RetractAtom::RetractAtom(KBModAddon& kbmod) : kbmod_addon(kbmod)
	{
		//addInputConstant();	
		//addInputConstant();			
	}



    // Standard function takes the query as an input and evaluates the associated function 
    void
    RetractAtom::execute(const Tuple & parms) throw(PluginError)
    {
	  Term fileName=parms[0];
	  //Term ruleToAdd=parms[1];
	  assert(parms.size()>=2);

	  std::string fileNameStr=fileName.getString();
	  fileNameStr=fileNameStr.substr(1, fileNameStr.size()-2);
	  std::string ruleStr="";//=ruleToAdd.getString();
	  //std::cout << "firstcharacter is: " << fileNameStr[0] << std::endl;
	  if(fileNameStr[0]!='\"' || fileNameStr[fileNameStr.size()-1]!='\"')
	  {
	  		throw PluginError("The first parameter of the atom #retract should be a quoted string!");
      	  }

	  
	  if(parms[1].getString()!="exact" || parms[1].getString()!= "predicate")
	  {
	  		throw PluginError("The second parameter of the atom #retract should be the option (i.e., exact or predicate)!");
          }
          
	  for(int i=2; i<parms.size(); i++)
	  {	
		if(parms[i].isInt())
		{
			std::ostringstream a_int;
			a_int<<parms[i];
			std::string int_str=a_int.str();
			ruleStr+=int_str;

		}
		else
		{	
			std::string ruleStr_par=parms[i].getString();
			 if(ruleStr[0]!='\"' || ruleStr[ruleStr.size()-1]!='\"')
		 	 {
		  		throw PluginError("The parameters of the atom #assert should be a quoted string or an integer!");
			  }
			ruleStr_par=ruleStr_par.substr(1, ruleStr.size()-2);
			ruleStr=ruleStr_par;
		}  	 	   
	  } 
  	
	  std::cout<<"RULE (retracted):" <<ruleStr<<std::endl;
	  std::ifstream input;
	  std::ofstream output;
	  output.open((fileNameStr+"_output").c_str());
	  input.open(fileNameStr.c_str());
	  std::string line;
	  if("predicate"==parms[1].getString())		//if a line contains rulestr such that it is of the form rulestr( or rulestr[ or "rulestr" only, the line is totaly omited. 
	  {
	  		while(getline(input,line))
			{
				if(line.find(ruleStr+"(")==std::string::npos && line.find(ruleStr+",")==std::string::npos && line.find(ruleStr+" ")==std::string::npos && line.find(ruleStr+"[")==std::string::npos)
				{
					output << line<< std::endl;
				}

			/*	if((line.find(":-")!=std::string::npos || line.find(":~")!=std::string::npos ||
					 line.find("<-")!=std::string::npos || line.find(ruleStr)!=0 ||
					 (line[ruleStr.size()-1]!='(' && line[ruleStr.size()-1]!='.' && line[ruleStr.size()-1]!='[')))
				{
					output << line << std::endl;
				}
			*/
			}
          }
	  else if("exact"==parms[1].getString())	//cleans the fact : "rulestr."
	  {
			while(getline(input,line)) 
			{
				if(!(line.find(":-")==std::string::npos && line.find(":~")==std::string::npos &&
					 line.find("<-")==std::string::npos && line==(ruleStr+".")))
				{
					output << line << std::endl;
				}
			}
	  }
	  


	  input.close();
	  output.close();
	  std::string command="rm "+fileNameStr+" && "+"mv "+fileNameStr+"_output "+ fileNameStr;
	  system(command.c_str());	  
	  
    }

    



 


  } // robot

} // dlvhex
