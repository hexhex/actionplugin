#ifndef _DLVHEX_Action_ActionCONVERTER_H_
#define _DLVHEX_Action_ActionCONVERTER_H_

#include <dlvhex/PluginInterface.h>

namespace dlvhex {
namespace aa {


  class ActionConverter : public PluginConverter
  {
	
    public:
    ActionConverter();
    

    void
      convert(std::istream& i, std::ostream& o);

  
  };

} // namespace aa
} // namespace dlvhex

#endif /* _DLVHEX_Action_ActionCONVERTER_H_ */
