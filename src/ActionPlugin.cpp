/* -*- C++ -*- */
/**
 * @file ActionPlugin.cpp
 * @author Selen Başol
 * @author Ozan Erdem
 * @date Sat Aug 1, 14:59:09 CET 2009
 *
 * @brief Corefile of the Action plugin
 *
 */

/** @mainpage dlvhex-robotplugin Source Documentation
 *
 * \section Overview
 * This plugin implements functions which make a robot move, and toggle an alarm.
 * The syntax is presented below:
 * \code
 * #robot[ACT,PARAM]{OPT,PREC}[W:L].
 * \endcode
 *
 * Where ACT is the name of the action, PARAM is the parameter that is associated
 * with the given action, OPT is the action option, PREC is the action precedence,
 * W is the action weight, and L is the action level.
 *
 * A simple example:
 *
 * \code
 * #robot[move,left]{b,1}[2:1].
 * \endcode
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "ActionPlugin.h"
#include <dlvhex/DLVProcess.h>

#include <dlvhex/ProgramCtx.h>
#include <iostream>

#include "dlvhex/globals.h"

namespace dlvhex {
  namespace aa {


  ActionPlugin::ActionPlugin()
  { }

  PluginConverter*
  ActionPlugin::createConverter()
  {
	converter = new ActionConverter();
	return converter;
  }

  OutputBuilder*
  ActionPlugin::createOutputBuilder()
  {
      outputbuilder=new ActionOutputBuilder();
      return outputbuilder;
  }	


  void
  ActionPlugin::getAtoms(AtomFunctionMap& a)
  {
  //  boost::shared_ptr<PluginAtom> robot(new ActionAtom(*this));
  //  a["robot"] = robot;
  }

ActionPlugin theActionPlugin;


  } // namespace aa
} // namespace dlvhex


extern "C"
dlvhex::aa::ActionPlugin*
PLUGINIMPORTFUNCTION()
{
  dlvhex::aa::theActionPlugin.setPluginName(PACKAGE_TARNAME);
  dlvhex::aa::theActionPlugin.setVersion(ACTIONPLUGIN_MAJOR,
				       ACTIONPLUGIN_MINOR,
				       ACTIONPLUGIN_MICRO);
  return &dlvhex::aa::theActionPlugin;
}
