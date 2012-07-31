/**
 * @file ActionPluginInterface.h
 * @author Stefano Germano
 *
 * @brief ...
 */

#ifndef ACTION_PLUGIN_INTERFACE_H_
#define ACTION_PLUGIN_INTERFACE_H_

#include "ActionPlugin.h"

#include "dlvhex2/PluginInterface.h"
#include "dlvhex2/PlatformDefinitions.h"
#include "dlvhex2/ProgramCtx.h"

DLVHEX_NAMESPACE_BEGIN

class ActionPluginInterface: public PluginInterface {

  public:

    ActionPluginInterface();
    ~ActionPluginInterface();

    // stored in ProgramCtx, accessed using getPluginEnvironment<ActionAtom>()
    class Environment: public PluginEnvironment {
      public:
        Environment();
        virtual ~Environment() {
        }
        ;
    };

    class PluginActionBase {
      public:
        PluginActionBase(std::string name) :
            name(name) {
        }
        virtual ~PluginActionBase() {
        }
        virtual void execute(Environment&, const Registry&, const Tuple&,
            InterpretationConstPtr &) = 0;
      protected:
        std::string name;
    };
    typedef boost::shared_ptr<PluginActionBase> PluginActionBasePtr;

    // CRTP pattern
    template<class Derived>
    class PluginActionAtom: public PluginAtom {
      public:

        typedef typename Derived::Environment Environment;

        PluginActionAtom(std::string name) :
            PluginAtom(name, false) {
        }
        virtual void retrieve(const typename Derived::Environment&, const Query&, Answer&) = 0;

      protected:
        virtual void retrieve(const Query& query, Answer& answer) {
          const typename Derived::Environment& env = query.ctx->getPluginEnvironment<Derived>();
          retrieve(env, query, answer);
        }
    };

    // CRTP pattern
    template<class Derived>
    class PluginAction: public PluginActionBase {
      public:

        typedef typename Derived::Environment Environment;

        PluginAction(std::string name) :
            PluginActionBase(name) {
        }
        virtual void execute(typename Derived::Environment&, const Registry&, const Tuple&,
            InterpretationConstPtr &) = 0;
    };

    /**
     * \brief Publish external computation sources to dlvhex.
     *
     * This is the central location where the user's atoms are made public.
     * dlvhex will call this function for all found plugins, which write their
     * atoms in the provided map. This map associates strings with pointers to
     * PluginAtom objects. The strings denote the name of the atom as it should
     * be used in the program.
     *
     * Override this method to publish your atoms.
     *
     * Example:
     *
     * \code
     *   std::vector<PluginAtomPtr>
     *   createAtoms(ProgramCtx& ctx) const
     *   {
     *    std::vector<PluginAtomPtr> ret;
     *      PluginAtomPtr newatom1(new MyAtom1);
     *      PluginAtomPtr newatom2(new MyAtom2);
     *      ret["newatom1"] = newatom1;
     *      ret["newatom2"] = newatom2;
     *      return ret;
     *   }
     * \endcode
     *
     * Here, we assume to have defined atoms MyAtom1 and MyAtom2
     * derived from PluginAtom. These atom can now be used in a
     * HEX-program with the predicate \b &newatom1[]() and
     * \b &newatom2[]().
     */
    virtual std::vector<PluginAtomPtr> createAtoms(ProgramCtx& ctx) const;

    virtual std::vector<PluginActionBasePtr> createActions(ProgramCtx& ctx) const;

    class BestModelSelector {
        BestModelSelector(std::string name) :
            name(name) {
        }
        ;
//        virtual ActionPlugin::CtxData::BestModelsContainer::iterator getBestModel(
//            const ActionPlugin::CtxData::BestModelsContainer&) = 0;
      protected:
        std::string name;
    };
    typedef boost::shared_ptr<BestModelSelector> BestModelSelectorPtr;

    virtual std::vector<BestModelSelectorPtr> getAllBestModelSelectors() const;

    virtual void setupProgramCtx(ProgramCtx& ctx) {
      //I don't know how I can call ActionPlugin
      // ActionPlugin -> registerActionPluginInterface(this);
    }

};
typedef boost::shared_ptr<ActionPluginInterface> ActionPluginInterfacePtr;

DLVHEX_NAMESPACE_END

#endif /* ACTION_PLUGIN_INTERFACE_H_ */
