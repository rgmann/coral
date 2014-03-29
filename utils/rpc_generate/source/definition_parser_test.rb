require File.join(File.dirname(__FILE__), 'definition_parser')

definitions = DefinitionParser.parse(ARGV[0])

definitions.each do |definition|
  puts definition.to_lineage_s
end

