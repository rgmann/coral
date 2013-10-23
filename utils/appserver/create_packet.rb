#!/usr/bin/env ruby

require File.expand_path(File.dirname(__FILE__) + '/packet_template')

arguments = Array.new

ARGV.each do |argument|
   arguments << argument
end

puts "Too few arguments!" unless arguments.length > 0

packet = PacketTemplate.new

packet.header_path = arguments[arguments.index('-hp')+1] if arguments.include?('-hp')
packet.implementation_path = arguments[arguments.index('-ip')+1] if arguments.include?('-ip')
packet.classname = arguments[arguments.index('-c')+1] if arguments.include?('-c')
packet.parent = arguments[arguments.index('-p')+1] if arguments.include?('-p')
packet.create()

