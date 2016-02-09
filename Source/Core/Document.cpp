#include "Document.hpp"

#include "DocumentIterator.hpp"
#include "SearchExpression.hpp"
#include "Selection.hpp"
#include "SelectionSet.hpp"

#include <iostream>
#include <regex>
#include <sstream>
#include <string>

namespace quip {
  Document::Document () {
  }
  
  Document::Document (const std::string & content) {
    for (std::size_t index = 0; index < content.size(); ++index) {
      std::size_t start = index;
      while (content[index] != '\n' && index < content.size()) {
        ++index;
      }
      
      m_rows.emplace_back(content.substr(start, index - start + 1));
    }
  }
  
  std::string Document::contents () const {
    std::ostringstream stream;
    for (const std::string & text : m_rows) {
      stream << text;
    }
    
    return stream.str();
  }
  
  DocumentIterator Document::begin () const {
    return DocumentIterator(*this, Location(0, 0));
  }
  
  DocumentIterator Document::end () const {
    return DocumentIterator(*this, Location(0, m_rows.size()));
  }
  
  const std::string & Document::path () const {
    return m_path;
  }
  
  void Document::setPath (const std::string & path) {
    m_path = path;
  }
  
  const std::string & Document::row (std::size_t index) const {
    return m_rows[index];
  }
  
  std::size_t Document::rows () const {
    return m_rows.size();
  }
  
  void Document::insert (SelectionSet & selections, const std::string & text) {
    // Split the text.
    std::vector<std::string> lines;
    for (std::size_t index = 0; index < text.size(); ++index) {
      std::size_t start = index;
      while (text[index] != '\n' && index < text.size()) {
        ++index;
      }
      
      lines.emplace_back(text.substr(start, index - start + 1));
    }
    
    // Account for potential trailing newline.
    if (lines.back().back() == '\n') {
      lines.emplace_back("");
    }
    
    std::int64_t rowDelta = 0;
    std::int64_t columnDelta = 0;
    std::size_t priorRow = rows();
    
    for (Selection & selection : selections) {
      // Reset column delta when changing rows.
      if (selection.lowerBound().row() != priorRow) {
        priorRow = selection.lowerBound().row();
        columnDelta = 0;
      }
      
      Location lower = selection.lowerBound().adjustBy(columnDelta, rowDelta);
      std::string prefix = m_rows[lower.row()].substr(0, lower.column());
      std::string suffix = m_rows[lower.row()].substr(lower.column());
      std::size_t insertRow = lower.row();
      m_rows[insertRow] = prefix + lines[0];
      columnDelta += lines[0].size();
      
      for (std::size_t lineIndex = 1; lineIndex < lines.size(); ++lineIndex) {
        m_rows.insert(m_rows.begin() + insertRow + 1, lines[lineIndex]);
        ++insertRow;
        ++rowDelta;
        columnDelta = 0;
      }
      
      m_rows[insertRow] += suffix;
      
      Location origin = selection.origin().adjustBy(columnDelta, rowDelta);
      if (lines.size() > 1) {
        origin = Location(0, origin.row());
      }
      
      selection.setOrigin(origin);
      selection.setExtent(selection.origin());
    }
  }
  
  void Document::erase (SelectionSet & selections) {
    std::int64_t rowDelta = 0;
    std::int64_t columnDelta = 0;
    std::size_t priorRow = rows();
    
    for (Selection & selection : selections) {
      // Reset column delta when changing rows.
      if (selection.lowerBound().row() != priorRow) {
        priorRow = selection.lowerBound().row();
        columnDelta = 0;
      }
      
      Location lower = selection.lowerBound().adjustBy(columnDelta, rowDelta);
      Location upper = selection.upperBound().adjustBy(columnDelta, rowDelta);
      std::uint64_t modified = selection.height();

      std::string prefix = m_rows[lower.row()].substr(0, lower.column());
      std::string suffix;
      if (upper.column() == m_rows[upper.row()].length() - 1) {
        suffix = m_rows[upper.row() + 1];
        ++modified;
      } else {
        suffix = m_rows[upper.row()].substr(upper.column() + 1);
      }
      
      std::string final = prefix + suffix;
      selection.setOrigin(selection.origin().adjustBy(columnDelta, 0));
      selection.setExtent(selection.origin());

      if (modified == 1) {
        columnDelta -= (upper.column() - lower.column() + 1);
      } else {
        columnDelta = upper.column();
        
        while (modified > 1) {
          m_rows.erase(m_rows.begin() + lower.row() + 1);
          --rowDelta;
          --modified;
        }
      }
      
      m_rows[lower.row()] = final;
    }
  }
  
  void Document::eraseBefore (SelectionSet & selections) {
    std::int64_t rowDelta = 0;
    std::int64_t columnDelta = 0;
    std::size_t priorRow = rows();
    
    for (Selection & selection : selections) {
      // Reset column delta when changing rows.
      if (selection.lowerBound().row() != priorRow) {
        priorRow = selection.lowerBound().row();
        columnDelta = 0;
      }
      
      // Don't delete before the first character.
      if (selection.lowerBound() == Location(0, 0)) {
        continue;
      }
      
      Location lower = selection.lowerBound().adjustBy(columnDelta, rowDelta);
      Location upper = selection.upperBound().adjustBy(columnDelta, rowDelta);
      std::uint64_t modified = selection.height();
      std::size_t destinationRow = lower.row();
      std::string prefix;
      if (lower.column() == 0) {
        const std::string & priorText = m_rows[lower.row() - 1];
        prefix = priorText.substr(0, priorText.length() - 1);
        --destinationRow;
        ++modified;
      } else {
        prefix = m_rows[lower.row()].substr(0, lower.column() - 1);
      }
      
      std::string suffix = m_rows[upper.row()].substr(upper.column());
      m_rows[destinationRow] = prefix + suffix;
      
      std::size_t destinationColumn = selection.origin().column() == 0 ? prefix.length() : selection.origin().column() - 1;
      selection.setOrigin(Location(destinationColumn, destinationRow));
      selection.setExtent(selection.origin());
      
      if (modified == 1) {
        columnDelta -= (upper.column() - lower.column() + 1);
      } else {
        columnDelta = upper.column();
        
        while (modified > 1) {
          m_rows.erase(m_rows.begin() + destinationRow + 1);
          --rowDelta;
          --modified;
        }
      }
    }
  }
  
  SelectionSet Document::matches (const SearchExpression & expression) const {
    typedef std::regex_iterator<DocumentIterator, char> RegexIterator;
    
    std::vector<Selection> results;
    if (expression.valid()) {
      RegexIterator cursor(begin(), end(), expression.pattern(), std::regex_constants::match_not_null);
      RegexIterator end;
      
      while (cursor != end) {
        auto match = *cursor;
        if (match.length() == 0) {
          // https://llvm.org/bugs/show_bug.cgi?id=21597 notes that libc++ doesn't currently
          // respect the "ignore empty matches" flag passed above. This can cause partially-entered
          // expressions containing \b assertions to generate an infinite loop, since incrementing
          // the iterator will never actually advance it. As a workaround, matches for a row are
          // aborted if any are empty, since that should only occur in the context of the libc++ bug.
          break;
        }
        
        for (std::size_t matchIndex = 0; matchIndex < match.size(); ++matchIndex) {
          Location origin = match[matchIndex].first.location();
          Location extent = std::prev(match[matchIndex].second).location();
          results.emplace_back(origin, extent);
        }
        
        ++cursor;
      }
    }
    
    return SelectionSet(results);
  }
}