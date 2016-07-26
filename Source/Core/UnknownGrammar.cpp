#include "UnknownGrammar.hpp"

#include "Document.hpp"

#include "pegtl.hh"

namespace quip {
  namespace UnknownGrammar {
    using namespace pegtl;

    struct KeywordQuip : pegtl_string_t("Quip") {};
    struct Anything : sor<KeywordQuip, any> {};
    struct File : until<eof, Anything> {};
    
    template<typename Rule>
    struct Action : nothing<Rule> {};
    
    template<>
    struct Action<KeywordQuip> {
      template<typename Input>
      static void apply (const Input & input, std::vector<AttributeRange> & results) {
        AttributeRange range(Keyword, input.byte_in_line(), input.size());
        results.push_back(range);
      }
    };
  
    std::vector<AttributeRange> parse (const std::string & row, const std::string & context) {
      std::vector<AttributeRange> results;
      parse_string<File, Action>(row, context, results);

      return results;
    }
  }
}