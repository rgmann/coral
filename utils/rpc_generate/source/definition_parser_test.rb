version = 'v2.'
require File.join(File.dirname(__FILE__), "#{version}definition_parser")

unless ARGV.count > 0
  puts "Please specify a file path."
  exit
end

collection = DefinitionParser.parse(ARGV[0])
#collection.select(:type => 'class') do |definition|
collection.select() do |definition|
#  puts definition.to_lineage_s
  puts definition.mangle
  puts ""
end

