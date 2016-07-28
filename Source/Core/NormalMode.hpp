#pragma once

#include "Mode.hpp"

namespace quip {
  struct EditContext;
  
  struct NormalMode : Mode {
    NormalMode ();
    
    std::string status () const override;

  private:
    void doSelectBeforePrimaryOrigin (EditContext & context);
    void doSelectBelowPrimaryExtent (EditContext & context);
    void doSelectAfterPrimaryExtent (EditContext & context);
    void doSelectAbovePrimaryOrigin (EditContext & context);
    
    void doSelectThisWord (EditContext & context);
    void doSelectNextWord (EditContext & context);
    void doSelectPriorWord (EditContext & context);
    
    void doSelectThisLine (EditContext & context);
    
    void enterJumpMode (EditContext & context);
    void enterEditMode (EditContext & context);
    void enterSearchMode (EditContext & context);
    
    void rotateSelectionForward (EditContext & context);
    void rotateSelectionBackward (EditContext & context);
    void collapseSelections (EditContext & context);
    
    void deleteSelections (EditContext & context);
    void changeSelections (EditContext & context);
    
    std::uint64_t m_virtualColumn;
  };
}