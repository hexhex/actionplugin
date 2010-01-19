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
 * @file   AddonContainer.h
 * @author Roman Schindlauer
 * @date   Thu Sep 1 17:21:53 2005
 * 
 * @brief  Container class for addons.
 * 
 * 
 */

#if !defined(_DLVHEX_ADDONCONTAINER_H)
#define _DLVHEX_ADDONCONTAINER_H

#include "dlvhex/PlatformDefinitions.h"
#include "dlvhex/PluginInterface.h"
#include "AddonAtom.h"

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>


DLVHEX_NAMESPACE_BEGIN


/**
 * @brief Collects and administrates all available addons.
 */
class DLVHEX_EXPORT AddonContainer
{


public:
  /// ctor
  explicit
  AddonContainer(const std::string& path);

  /// copy ctor
  AddonContainer(const AddonContainer&);

  /// dtor
  ~AddonContainer();
   
  
  /// get the AddonContainer singleton instance
  static AddonContainer*
  instance(const std::string&);

  /**
   * @brief Loads a library and accesses its addon-interface.
   */
  std::vector<AddonInterface*>
  importAddons();

  /**
   * @brief returns a addon-atom object corresponding to a name.
   */
  boost::shared_ptr<AddonAtom>
  getAtom(const std::string& name) const;


private:

  /// singleton instance
  static AddonContainer* theContainer;

  /// list of addons
  std::vector<std::string> addonList;

  /**
   * @brief Associative map of external atoms provided by addons.
   */
  AddonInterface::AddonAtomFunctionMap addonAtoms;

};


DLVHEX_NAMESPACE_END

#endif /* _DLVHEX_ADDONCONTAINER_H */


// Local Variables:
// mode: C++
// End:
