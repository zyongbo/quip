#pragma once

#include "SelectionSet.hpp"
#include "Transaction.hpp"

#include <string>
#include <memory>

namespace quip {
  struct SelectionSet;
  
  struct EraseTransaction : Transaction {
    EraseTransaction (const SelectionSet & selections);
    ~EraseTransaction ();
    
    void perform (EditContext & context) override;
    void rollback (EditContext & context) override;
    
    static std::shared_ptr<Transaction> create (const SelectionSet & selections);
    
  private:
    SelectionSet m_selections;
    std::vector<std::string> m_text;
  };
}