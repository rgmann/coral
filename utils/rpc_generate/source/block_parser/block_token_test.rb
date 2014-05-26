version = ''
require File.join(File.dirname(__FILE__), "#{version}block_tree")

statement = "optional PhoneType type = 2 [default = HOME];" 
statement = "void func(int param1, float param2)"
#statement = File.read(ARGV[0])

puts "Tokenizing \"#{statement}\":"
tokens = BlockTree.identify_blocks(statement)
puts tokens.inspect

