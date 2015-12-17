#include "Mode.hpp"

#include "EditContext.hpp"

namespace quip {
  Mode::~Mode () {
  }
  
  bool Mode::processKey (const KeyStroke & keyStroke, EditContext & context) {
    auto cursor = m_mappings.find(keyStroke.key());
    if (cursor != std::end(m_mappings)) {
      cursor->second(context);
      return true;
    }
    
    return onUnmappedKey(keyStroke, context);
  }
  
  bool Mode::onUnmappedKey (const KeyStroke & keyStroke, EditContext & context) {
    return false;
  }
}