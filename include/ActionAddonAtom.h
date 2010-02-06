/* dlvhex -- Answer-Set Programming with external interfaces.
 * Copyright (C) 
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
 * \file   PluginInterface.h
 * \author Selen Basol, Ozan Erdem
 * \date   Thu Oct 1 15:36:10 2009
 *
 * \brief Declaration of Classes ActionAddonAtom, PluginRewriter,
 * and PluginInterface.
 */




#if !defined(_DLVHEX_ADDONATOM_H)
#define _DLVHEX_ADDONATOM_H

#include "dlvhex/PlatformDefinitions.h"

#include "dlvhex/Term.h"
#include "dlvhex/Atom.h"
#include "dlvhex/AtomSet.h"
#include "dlvhex/Error.h"

#include <map>
#include <string>
#include <iosfwd>

#include <boost/shared_ptr.hpp>


#define PLUGINIMPORTFUNCTION importPlugin
#define PLUGINIMPORTFUNCTIONSTRING "importPlugin"


DLVHEX_NAMESPACE_BEGIN

// forward declarations
class Program;
class NodeGraph;
class OutputBuilder;
class ProgramCtx;


/**
 * \brief Converter class.
 *
 * A converter can be seen as a preprocessor, which receives the raw input
 * program. A converter can either decide to parse the program and return a
 * well-formed HEX-program or simply leave the input untouched.
 */




/**
 * \brief Interface class for external Atoms.
 *
 * \ingroup pluginframework
 *
 * An external atom is represented by a subclassed ActionAddonAtom. The actual
 * evaluation function of the atom is realized in the ActionAddonAtom::retrieve()
 * method. In the constructor, the user must specify the types of input terms
 * and the output arity of the atom.
 */
class DLVHEX_EXPORT ActionAddonAtom
{
public:

    /**
     * \brief Query class for wrapping the input of an external atom call.
     */
    class DLVHEX_EXPORT Query
    {
    public:
        /**
         * \brief Query Constructor.
         *
         * A query has three components:
         * - The input interpretation,
         * - the input arguments, and
         * - the output tuple.
		 * .
         * The input arguments are the ground terms of the input list. The
         * output tuple corresponds to the atom's output list: If it contains
         * variables, the query will be a functional one for those missing
         * values; if it is nullary or completely ground, the query will be a
         * boolean one. Either way, the answer will contain exactly those tuples
         * that are in the output of the atom's function for the interpretation
         * and the input arguments.
         */
        Query(const Tuple&);

        /**
         * \brief Returns the input interpretation.
		 *
		 * An external atom is evaluated w.r.t. the 
		 * The interpretation is one part of the input to an external atom,
		 * which is evaluated
         */
        const AtomSet&
        getInterpretation() const;

        /**
         * \brief Returns the input parameter tuple.
         */
        const Tuple&
        getInputTuple() const;

        /**
         * \brief Return the input pattern.
         */
        const Tuple&
        getPatternTuple() const;


    private:

        Tuple input;
    };





    /**
     * \brief Type of input parameter.
     *
     * Currently, two types of input parameters can be specified: PREDICATE and
     * CONSTANT.
     * An input argument of type PREDICATE means that the atom needs those facts
     * of the interpretation whose predicate match the value of this argument.
     * An input argument of type CONSTANT means that only its value is relevent
     * to the external atom, regardless of the interpretation.
     * Specifying the input parameters' types is necessary for reducing the
     * interpretation that will be passed to the external atom as well as
     * improving the dependency information used by the internal
     * evaluation strategies of dlvhex.
     */
    typedef enum { PREDICATE, CONSTANT, TUPLE } InputType;


protected:

    /// Ctor.
    ActionAddonAtom()
    { }


public:

    /// Dtor
    virtual
    ~ActionAddonAtom()
    { }


    /**
     * \brief Adds an input parameter of type PREDICATE.
     *
     * See InputType.
     */
    void
    addInputPredicate();

    /**
     * \brief Adds an input parameter of type CONSTANT.
     *
     * See InputType.
     */
    void
    addInputConstant();
	
    void addConstant();
    /**
     * \brief Adds an input parameter of type TUPLE.
     *
     * See InputType.
     */
    void
    addInputTuple();

    /**
     * \brief Checks the input arity of the external atom against the specified
	 * one.
     *
     * The input arity follows from the number of specified predicate types (see
     * addInputPredicate and addInputConstant).
     */
    bool
    checkInputArity(unsigned arity) const;

 
    /**
     * \brief Retrieve answer object according to a query.
     */
    virtual void
    execute(const Tuple&) throw (PluginError) = 0;


    /**
     * \brief Returns the type of the input argument specified by position
     * (starting with 0).
     */
    InputType
    getInputType(unsigned index) const;

    /**
     * @return inputType
     */
    const std::vector<InputType>&
    getInputTypes() const;

private:

    /**
     * \brief Number of input arguments.
     */
    unsigned inputSize;

    /**
     * \brief Type of each input argument.
     */
    std::vector<InputType> inputType;

};

/**
 * \brief Factory base class for representing addons and creating necessary objects.
 *
 */
class DLVHEX_EXPORT ActionAddonInterface
{
 protected:
  
  /// Ctor.
  ActionAddonInterface()
    : addonName(""),
    versionMajor(0),
    versionMinor(0),
    versionMicro(0)
  { }

  std::string addonName;

  unsigned versionMajor;
  unsigned versionMinor;
  unsigned versionMicro;

public:
    /// Dtor.
    virtual
    ~ActionAddonInterface()
    { }

    /**
     * \brief Associates atom names with function pointers.
     */
    typedef std::map<std::string, boost::shared_ptr<ActionAddonAtom> > ActionAddonAtomFunctionMap;
   
    /**
     * \brief Fills a mapping from atom names to the addon's atom objects.
     *
     * This is the central location where the user's atoms are made public.
     * dlvhex will call this function for all found addons, which write their
     * atoms in the provided map. This map associates strings with pointers to
     * ActionAddonAtom objects. The strings denote the name of the atom as it should
     * be used in the program.
     *
     * Example:
     *
     * \code
     * void
     * getAtoms(ActionAddonAtomFunctionMap& a)
     * {
     *     boost::shared_ptr<ActionAddonAtom> newatom(new MyAtom);
     *     a["newatom"] = newatom;
     * }
     * \endcode
     *
     * Here, we assume to have defined an atom MyAtom derived from ActionAddonAtom.
     * This atom can now be used in a hex-program with the predicate \b
     * &newatom.
     *
     * Naturally, more than one atoms can be registered here:
     *
     * \code
     * boost::shared_ptr<ActionAddonAtom> split(new SplitAtom);
     * boost::shared_ptr<ActionAddonAtom> concat(new ConcatAtom);
     * boost::shared_ptr<ActionAddonAtom> substr(new SubstringAtom);
     * a["split"] = split;
     * a["concat"] = concat;
     * a["substr"] = substr;
     * \endcode
     */
    virtual void
    getAtoms(ActionAddonAtomFunctionMap&)
    { }

    /**
     * \brief Propagates dlvhex program options to the addon.
     *
     * Each option known to the addon must be deleted from the vector. dlvhex
     * will exit with an error if unknown options are left in the vector after
     * all addons have been processed.
     * If the first parameter is true, then help was requested. The addon must
     * write its help output into the given stream;
     */
    virtual void
    setOptions(bool, std::vector<std::string>&, std::ostream&)
    { }

    /**
     * \brief Set addon name.
     *
     * The addon name will be displayed when dlvhex loads the
     * addon. This method is not supposed to be overridden, but only
     * called in the PLUGINIMPORTFUNCTION() (see Section \ref
     * importing).
     */
    void
    setActionAddonName(const std::string& name)
    {
      this->addonName = name;
    }

    /**
     * \brief Set addon version.
     *
     * The version number will be displayed when dlvhex loads the addon. It can
     * be used to check whether the right version is loaded. This method is not
     * supposed to be overridden, but only called in the PLUGINIMPORTFUNCTION()
     * (see Section \ref importing).
     */
    void
    setVersion(unsigned major, unsigned minor, unsigned micro)
    {
      this->versionMajor = major;
      this->versionMinor = minor;
      this->versionMicro = micro;
    }

    const std::string&
    getActionAddonName() const
    {
      return this->addonName;
    }

    unsigned
    getVersionMajor() const
    {
      return this->versionMajor;
    }

    unsigned
    getVersionMinor() const
    {
      return this->versionMinor;
    }

    unsigned
    getVersionMicro() const
    {
      return this->versionMicro;
    }
};



DLVHEX_NAMESPACE_END

#endif // _DLVHEX_ADDONATOM_H


// Local Variables:
// mode: C++
// End:
