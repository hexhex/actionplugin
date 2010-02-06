/* dlvhex -- Answer-Set Programming with external actions.
 * 
 * 
 * This file is part of dlvhex Action Plugin.
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
 * @file   ActionAddonContainer.cpp
 * @author Selen Basol, Ozan Erdem
 * @date   Thu D 1 17:25:55 2009
 * 
 * @brief  Container class for action addons.
 * 
 * 
 */

#include "dlvhex/Error.h"
#include "ActionAddonAtom.h"
#include "ActionAddonContainer.h"

#include <ltdl.h>

#include <sys/types.h>
#include <dirent.h>
#include <pwd.h>

#include <iostream>
#include <sstream>

DLVHEX_NAMESPACE_BEGIN

typedef ActionAddonInterface* (*t_import)();

ActionAddonContainer* ActionAddonContainer::theContainer = 0;

ActionAddonContainer*
ActionAddonContainer::instance(const std::string& optionPath)
{
  if (ActionAddonContainer::theContainer == 0)
    {
      ActionAddonContainer::theContainer = new ActionAddonContainer(optionPath);
    }

  return ActionAddonContainer::theContainer;
}


int
findaddons(const char* filename, lt_ptr data)
{
  std::vector<std::string>* addonlist = reinterpret_cast<std::vector<std::string>*>(data);

  std::string fn(filename);
  std::string::size_type base = fn.find_last_of("/");

  // if basename starts with 'libdlvhex', then we should have a addon here
  /// @todo we could lt_dlopen the file here, to check if it is really a addon
  if (fn.substr(base).find("/libaction") == 0)
    {
      addonlist->push_back(fn);
    }
  return 0;
}

ActionAddonContainer::ActionAddonContainer(const ActionAddonContainer& pc)
  : addonList(pc.addonList),
    addonAtoms(pc.addonAtoms)
{ }

ActionAddonContainer::ActionAddonContainer(const std::string& optionPath)
{
  if (lt_dlinit())
    {
      throw GeneralError("Could not initialize libltdl");
    }

  //
  // now look into the user's home, and into the global addon directory
  //
  std::stringstream searchpath;

  const char* homedir = ::getpwuid(::geteuid())->pw_dir;
//std::cout << "sys plugin dir is: " << SYS_PLUGIN_DIR << std::endl;
//std::cout << "user plugin dir is: " << USER_PLUGIN_DIR << std::endl;
  searchpath << optionPath << ':'
	     << homedir << "/" USER_PLUGIN_DIR << ':'
	     << SYS_PLUGIN_DIR;

  if (lt_dlsetsearchpath(searchpath.str().c_str()))
    {
      throw GeneralError("Could not set libltdl search path: " + searchpath.str());
    }

  // search the directory search paths for addons and setup addonList
  lt_dlforeachfile(NULL, findaddons, &this->addonList);
}


ActionAddonContainer::~ActionAddonContainer()
{
  ///@todo this does not work, we have to include specific unloading functions in the addons
  /* if (lt_dlexit())
    {
      std::cerr << "lt_dlexit() failed" << std::endl;
      } */
}


std::vector<ActionAddonInterface*>
ActionAddonContainer::importActionAddons()
{
  ///@todo this is not that good
  std::vector<ActionAddonInterface*> addons;
//  lt_dlhandle a_handle = lt_dlopenext(

  for (std::vector<std::string>::const_iterator it = addonList.begin();
       it != addonList.end(); ++it)
    {
      lt_dlhandle dlHandle = lt_dlopenext(it->c_str());

      ///@todo if we cannot open the addon, we bail out. maybe we
      ///should gracefully resuscicate ourselves
      if (dlHandle == NULL)
	{
	  throw FatalError("Cannot open library " + *it + ": " + lt_dlerror());
	}

      t_import getaddon = (t_import) lt_dlsym(dlHandle, PLUGINIMPORTFUNCTIONSTRING);
      
      if (getaddon != NULL)
	{
	  ActionAddonInterface::ActionAddonAtomFunctionMap pa;

	  ActionAddonInterface* addon = getaddon();

	  addons.push_back(addon);

	  addon->getAtoms(pa);

	  for(ActionAddonInterface::ActionAddonAtomFunctionMap::const_iterator it = pa.begin();
	      it != pa.end();
	      ++it)
	    {
	      // first come, first serve
	      if (addonAtoms.find(it->first) == addonAtoms.end())
		{
		  addonAtoms[it->first] = it->second;

		//std::cout<<"an plugin is being registered. "<<it->first<<std::endl;
		}
	      else
		{
		  ///@todo is this a warning, or a proper (installation) error?
		  std::cerr << "Warning: the external atom " << it->first
			    << " is already loaded." << std::endl;
		}
	    }
	}
    }

  return addons;
}


boost::shared_ptr<ActionAddonAtom>
ActionAddonContainer::getAtom(const std::string& name) const
{
 ActionAddonInterface::ActionAddonAtomFunctionMap::const_iterator pa = addonAtoms.find(name);

  if (pa == addonAtoms.end())
    {
      return boost::shared_ptr<ActionAddonAtom>();
    }
    
  return pa->second;
}

DLVHEX_NAMESPACE_END

// Local Variables:
// mode: C++
// End:
