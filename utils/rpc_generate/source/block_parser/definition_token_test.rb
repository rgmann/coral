version = ''
require File.join(File.dirname(__FILE__), "#{version}definition_parser")

statement = "optional PhoneType type = 2 [default = HOME];" 
statement = "void func<int param1, float param2>"

puts "Tokenizing \"#{statement}\":"
tokens = DefinitionParser.tokenize(statement)
puts tokens.inspect
puts DefinitionParser::Bracket.inner_text(tokens[2], :chevron)

