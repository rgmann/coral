#!/usr/bin/ruby

require File.join(File.expand_path('./'), 'text_template.rb')
require File.join(File.expand_path('./'), 'field.rb')
require File.join(File.expand_path('./'), 'structure.rb')
require File.join(File.expand_path('./'), 'rpc_parameter_list.rb')
require File.join(File.expand_path('./'), 'rpc_resource_action.rb')
require File.join(File.expand_path('./'), 'rpc_resource.rb')


class RpcGenerator

  attr_accessor :resources
  attr_accessor :structures

  def initialize
    @resources = Array.new
    @structures = Hash.new
  end

  def self.strip_comments(text)
    text.gsub!(/#[^\n]*/, '')
  end

  def self.run(args)

    raise "Too few args" if args.count < 3

    def_text = ''
    def_path = args[0]
    decl_out_path = args[1]
    def_out_path = args[2]

    generator = RpcGenerator.new
    declarations = Array.new
    definitions = Array.new

    begin
      File.open(def_path, 'r') { |io| def_text = io.read }
    rescue Exception => e
      puts "rpc_generate: #{e.message}"
      return
    end

    # Remove comments
    strip_comments(def_text)

    def_text.scan(RpcResource::REGEX) do |match|
      generator.resources << RpcResource.new(match)
    end

    def_text.scan(Structure::REGEX) do |match|
      structure = Structure.new(:text => match)
      if generator.structures[structure.name]
        raise "Duplicate definition for structure #{structure.name}"
      else
        generator.structures[structure.name] = structure
      end
    end

    generator.resources.each do |resource|
      resource.declarations.each {|decl| declarations << decl }
      resource.definitions.each {|defn| definitions << defn }
    end

    generator.structures.values.each do |structure|
      declarations << structure.declaration
      definitions << structure.definition
    end

    declarations.each do |file|
      File.open(File.join(decl_out_path, file[:name]), 'w') do |io|
        io.write(file[:text].join)
      end
    end

    definitions.each do |file|
      File.open(File.join(def_out_path, file[:name]), 'w') do |io|
        io.write(file[:text].join)
      end
    end
  end
end

RpcGenerator.run(ARGV)

