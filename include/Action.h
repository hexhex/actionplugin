/**
 * @file Action.h
 * @author Stefano Germano
 *
 * @brief ...
 */

#ifndef ACTION_H_
#define ACTION_H_

DLVHEX_NAMESPACE_BEGIN

class Action {
  public:
    Action(std::string predicate, dlvhex::ID aux_id);
    virtual ~Action();

    dlvhex::ID getAuxId() const;
    std::string getPredicate() const;

  protected:
    std::string predicate;
    dlvhex::ID aux_id;
};

DLVHEX_NAMESPACE_END

#endif /* ACTION_H_ */
