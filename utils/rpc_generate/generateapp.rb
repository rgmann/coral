#!/usr/bin/ruby

require File.join(File.expand_path('./'), 'text_template.rb')
require File.join(File.expand_path('./'), 'field.rb')
require File.join(File.expand_path('./'), 'rpc_struct.rb')
require File.join(File.expand_path('./'), 'rpc_resource_action.rb')
require File.join(File.expand_path('./'), 'rpc_resource.rb')


class RpcGenerator

  attr_accessor :resources
  attr_accessor :actions

  def initialize
    @resources = Array.new
    @actions = Array.new
  end

  def self.run(args)

    raise "Too few args" if args.count < 3

    def_text = ''
    def_path = args[0]
    decl_out_path = args[1]
    def_out_path = args[2]

    generator = RpcGenerator.new

    begin
      File.open(def_path, 'r') { |io| def_text = io.read }
    rescue Exception => e
      puts "rpc_generate: #{e.message}"
      return
    end

    def_text.scan(RpcResource::REGEX) do |match|
      generator.resources << RpcResource.new(match)
    end

    def_text.scan(RpcStruct::REGEX) do |match|
      puts "action: #{match.inspect}"
      generator.actions << RpcStruct.new(:text => match)
    end

    generator.resources.each do |resource|
      resource.generate(decl_out_path, def_out_path)
    end

    generator.actions.each do |action|
      action.generate(decl_out_path, def_out_path)
    end
  end
end

# Parse resource definitions
#service_text.scan(RpcResource::RESOURCE_REGEX) do |match|
#  service = RpcResource.new
#  service.parse(match)
#  service.generate(ARGV[1], ARGV[2])
#end

RpcGenerator.run(ARGV)

# Parse structure definitions

