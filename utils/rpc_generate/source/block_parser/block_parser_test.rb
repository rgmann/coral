version = ''
require File.join(File.dirname(__FILE__), "#{version}block_parser")
require File.join(File.dirname(__FILE__), "#{version}tokenizer")

unless ARGV.count > 0
  puts "Please specify a file path."
  exit
end

allow_anonymous = true
collection = BlockParser.parse(ARGV[0], allow_anonymous: allow_anonymous)
#filter = {:type => 'resource'}
filter = {:depth => 0}
#filter = {}
collection.select(filter) do |block|
  puts block.mangle
#  next unless block.name == "global"
  puts "members:"
  block.members do |member|
    puts " #{Tokenizer.tokens(member).inspect}"
  end
  puts ""
end

