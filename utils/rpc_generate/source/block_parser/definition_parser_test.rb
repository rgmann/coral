version = ''
require File.join(File.dirname(__FILE__), "#{version}definition_parser")

unless ARGV.count > 0
  puts "Please specify a file path."
  exit
end

collection = DefinitionParser.parse(ARGV[0])
filter = {}#{:type => 'resource'}
collection.select(filter) do |definition|
  puts definition.mangle
  puts "members:"
  definition.members(/\b\w+\s+=\s+\w+/) do |member|
    puts " #{member}"
  end
  puts ""
end

