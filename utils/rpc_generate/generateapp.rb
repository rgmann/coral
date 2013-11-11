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

    raise "Too few args" if args.count < 2

    def_text = ''
    def_path = args[0]
    project_path = args[1]

    # project_directory
    #  |
    #  -- server_directory
    #  |   |
    #  |   -- include
    #  |   |
    #  |   -- source
    #  |   |
    #  |   -- rpc_include
    #  |   |
    #  |   -- rpc_source
    #  |
    #  -- common_directory
    #  |   |
    #  |   -- include
    #  |   |
    #  |   -- source
    #  |   |
    #  |   -- rpc_include
    #  |   |
    #  |   -- rpc_source
    #  |
    #  -- client_directory
    #      |
    #      -- include
    #      |
    #      -- source
    #      |
    #      -- rpc_include
    #      |
    #      -- rpc_source

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

    # Create output directories if they do not already exist.
    ['server', 'common', 'client'].each do |dirname|
      dirpath = File.join(project_path, dirname)
      Dir.mkdir(dirpath) unless Dir.exists?(dirpath)
      
      ['rpc_includes', 'includes', 'rpc_source', 'source'].each do |subdirname|
        subdirpath = File.join(dirpath, subdirname)
        Dir.mkdir(subdirpath) unless Dir.exists?(subdirpath)
      end
    end

    declarations.each do |file|
      path = File.join(project_path, file[:type].to_s, 'rpc_include', file[:name])
      File.open(path, 'w') { |io| io.write(file[:text].join) }
    end

    definitions.each do |file|
      path = File.join(project_path, file[:type].to_s, 'rpc_source', file[:name])
      File.open(path, 'w') { |io| io.write(file[:text].join) }
    end
  end
end

RpcGenerator.run(ARGV)

