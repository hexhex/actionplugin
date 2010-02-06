/* -*- C++ -*- */
/**
 * @file ActionPlugin.hpp
 * @author Selen Başol
 * @author Ozan Erdem
 * @date Sat Aug 1, 14:59:09 CET 2009
 *
 * @brief Header file for ActionPlugin
 *
 */
#ifndef _Action_PLUGIN_H
#define _Action_PLUGIN_H

#include <dlvhex/PluginInterface.h>
#include "ActionConverter.h"
#include "ActionOutputBuilder.h"
#include <dlvhex/Error.h>


namespace dlvhex {
  namespace aa {

class ActionPlugin : public PluginInterface
{

public:
  ActionPlugin();
  PluginConverter*  createConverter();
  OutputBuilder* createOutputBuilder();

private:

  void
  getAtoms(AtomFunctionMap& a);
  ActionConverter* converter;
  ActionOutputBuilder* outputbuilder;

};  // class ActionPlugin



  } // namespace aa
} // namespace dlvhex


#endif /* _Action_PLUGIN_H */
