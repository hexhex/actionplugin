/* -*- C++ -*- */
/**
 * @file RetractAtom.hpp
 * @author Selen Başol
 * @author Ozan Erdem
 * @date Wed Aug 19, 04:28:43 CET 2009
 *
 * @brief Header file for RetractAtom
 *
 */
#ifndef _RETRACT_ATOM_H
#define _RETRACT_ATOM_H

#include <dlvhex/PluginInterface.h>
#include <dlvhex/Error.h>

namespace dlvhex {
  namespace kbmod {

// forward declarations
class KBModAddon;

/**
 * Class for the kbmod atom.
 *
**/


class RetractAtom : public AddonAtom
{

protected:
  KBModAddon& kbmod_addon;

public:
  /**
   * Constructor of KBModatom. Defines the inputConstant of the plugin atom.
   */
  RetractAtom(KBModAddon& kbmod);

  virtual
  ~RetractAtom();

  /**
   * Retrieves the resulting kbmod tuples in the answer object for the input query.
   */
  virtual void
  execute(const Tuple & parms) throw(PluginError);







};

  } // namespace kbmod
} // namespace dlvhex

#endif /* RETRACT_ATOM_H */
