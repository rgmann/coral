version = ''
require File.join(File.dirname(__FILE__), "#{version}definition_parser")

unless ARGV.count > 0
  puts "Please specify a file path."
  exit
end

puts DefinitionParser.remove_comments(File.read(ARGV[0]))

