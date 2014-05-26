#!/usr/bin/ruby

require 'FileUtils' # For copying/moving files
require 'open3'     # For running protoc
#require File.join(File.expand_path('./'), 'parse_helper.rb')
#require File.join(File.expand_path('./'), 'pb_message')
#require File.join(File.expand_path('./'), 'field.rb')
#require File.join(File.expand_path('./'), 'rpc_parameter_list.rb')
#require File.join(File.expand_path('./'), 'rpc_resource.rb')
require File.join(File.dirname(__FILE__), 'block_parser', 'block_parser')
require File.join(File.dirname(__FILE__), 'block_parser', 'tokenizer')

class RpcGenerator

  attr_accessor :resources
  attr_accessor :messages
  attr_accessor :protos
  attr_accessor :project_path
  attr_accessor :definition_path
  attr_accessor :namespace

  def initialize
    @namespace = nil
    @resources = Array.new
    @messages = Hash.new
    @project_path = nil
    @definition_path = nil
    @ext_proto_paths = Array.new
  end

  def self.strip_comments(text)
    text.gsub!(/#[^\n]*/, '')
  end

  def create_project_tree
    # Create output directories if they do not already exist.
    ['server', 'common', 'client'].each do |dirname|
      dirpath = File.join(@project_path, dirname)
      Dir.mkdir(dirpath) unless Dir.exists?(dirpath)

      ['rpc_include', 'include', 'rpc_source', 'source'].each do |subdirname|
        subdirpath = File.join(dirpath, subdirname)
        Dir.mkdir(subdirpath) unless Dir.exists?(subdirpath)
      end
    end
  end

  def add_attribute(tokens)
    case tokens.first
    when 'import_proto'
      puts "import_path tokens = #{tokens.inspect}"
      unless tokens.count > 1
        raise "'import_proto' expects a path to a proto definition."
      end
      unless tokens[1].class == Array and [:single_quote, :double_quote].include? tokens[1][0]
        raise "'import_proto' should specify path in single or double quotes"
      end
      @ext_proto_paths << tokens[1][1].first
    when 'namespace'
      unless tokens.count > 1 and tokens[1].class == String
        raise "'namspace' should be followed by the namespace name"
      end
      @namespace = tokens[1]
    when 'tab_size'
      puts "tab_size: #{tokens.inspect}"
    else
    end
  end

  def set_block_tree(block_tree)
    @block_tree = block_tree

    # Get members of the global block.  Members of the global block set global
    # attributes.
    @block_tree.select({depth: 0}) do |block|
      block.members do |member|
        add_attribute(Tokenizer.tokens(member)) 
      end
    end

    # If the namespace was not set explicitly, set it to the resource filename.
    @namespace = File.basename(@definition_path, 'res') unless @namespace
    @namespace = "rpc_#{@namespace}"
  end

  def process_message_blocks
    @block_tree.select({depth: 1, type: 'message'}) do |block|
      @messages[name] = PbMessage.new(block, @namespace)
    end
  end

  def process_resource_blocks
    @template = TemplateCollection.new
    @block_tree.select({depth: 1: type: 'resource'}) do |block|
      @resources << RpcResource.new(block, @templates)
    end
  end

  def build_message_proto
    intermediate_dir = File.join(@project_path, 'intermediate')
    proto_path = File.join(intermediate_dir, "#{@namespace}.proto")

    Dir.mkdir(intermediate_dir) unless Dir.exists?(intermediate_dir)

    File.open(proto_path, 'w+') do |io|

      @ext_proto_paths.each do |ext_proto_path|
        io.puts "import \"#{ext_proto_path}\";"
      end

      io.puts "package #{@namespace};" if @namespace

      io.puts "\n\n"

      @messages.values.each do |message|
        io.write(message.to_message_decl) if message.to_message_decl
        io.puts "\n"
      end
    end

    if File.exists?(proto_path)

      # Build the protoc command.
      protoc_cmd = "protoc "
      protoc_cmd << "--proto_path=#{intermediate_dir} "
      protoc_cmd << "--cpp_out=#{intermediate_dir} "
      protoc_cmd << "--java_out=#{intermediate_dir} "
      protoc_cmd << "--python_out=#{intermediate_dir} "
      protoc_cmd << proto_path

      protoc_success = false
      protoc_stderr = ''
      Open3.popen3(protoc_cmd) do |stdin, stdout, stderr, wait_thr|
        protoc_stderr = stderr.gets(nil)
        protoc_success = (wait_thr.value == 0)
      end

      if not protoc_success
        error_msg = "Protoc failed to process the intermediate proto defintion:\n"
        error_msg << "  (See #{proto_path})\n"
        if not protoc_stderr.empty?
          protoc_stderr.lines.each do |line|
            error_msg << "  #{line.strip}\n"
          end
        end

        return [false, error_msg]
      end

      sources = []
      Dir.entries(intermediate_dir).each do |entry|
        sources << entry if entry =~ /[\w_]+.pb.(h|cc)/
      end

      files_created = true
      files_created &= sources.include?("#{@namespace}.pb.h")
      files_created &= sources.include?("#{@namespace}.pb.cc")

      if not files_created
        return [false, "Failed to generate protocol buffer definitions."]
      else
        hdr_path = File.join(@project_path, "common", "rpc_include")
        src_path = File.join(@project_path, "common", "rpc_source")

        sources.each do |entry|
          dst_path = nil
          dst_path = File.join(hdr_path, entry) if entry =~ /[\w_]+.pb.h/
          dst_path = File.join(src_path, entry) if entry =~ /[\w_]+.pb.cc/

          if dst_path
            #puts "FileUtils.cp(#{File.join(intermediate_dir, entry)}, #{dst_path})"
            FileUtils.cp(File.join(intermediate_dir, entry), dst_path)
          end
        end
      end
    end

    [true, nil]
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
    #  |   |
    #  |   -- include
    #  |   |
    #  |   -- source
    #  |   |
    #  |   -- rpc_include
    #  |   |
    #  |   -- rpc_source
    #  |
    #  -- intermediate

    generator = RpcGenerator.new
    generator.definition_path = args[0]
    generator.project_path = args[1]

    declarations = Array.new
    definitions = Array.new

#    begin
#      File.open(def_path, 'r') { |io| def_text = io.read }
#    rescue Exception => e
#      puts "rpc_generate: #{e.message}"
#      return
#    end

    block_tree = nil
    begin
      block_tree = BlockParser.parse(def_path)
    rescue Exception => e
      puts "rpc_generate: #{e.message}"
      exit
    end

#    generator.create_project_tree
    generator.set_block_tree(block_tree)
#    generator.get_attributes(def_text)
    RpcResource.set_namespace(generator.namespace)


    # Retrieve all message definition blocks.
#    ParseHelper.blocks(def_text) do |type, name, body|
#      if type == "message"
#        generator.messages[name] = PbMessage.new(name, generator.namespace, body)
#      end
#    end
    generator.process_message_blocks
    RpcResourceAction.set_message_list(generator.messages)

    # Retrieve all resource definition blocks.
#    template_collection = TemplateCollection.new
#    ParseHelper.blocks(def_text) do |type, name, body|
#      if type == "resource"
#        generator.resources << RpcResource.new(template_collection, name, body)
#      end
#    end
    generator.process_resource_blocks

    generator.resources.each do |resource|
      resource.declarations.each {|decl| declarations << decl }
      resource.definitions.each {|defn| definitions << defn }
      #generator.messages.concat(resource.messages)
    end

    # Generate the application proto file.
    success, msg = generator.build_message_proto
    if not success
      puts msg if msg
      return false
    end

    file_report_path = generator.project_path
    file_report_path = File.join(file_report_path, 'intermediate')
    file_report_path = File.join(file_report_path, 'file_gen_summary.txt')
    file_report = File.open(file_report_path, 'w+')

    declarations.each do |file|
      path = File.join(generator.project_path, file[:type].to_s, 'rpc_include', file[:name])
      File.open(path, 'w') { |io| io.write(file[:text].join) }
      file_report.puts path
    end

    definitions.each do |file|
      path = File.join(generator.project_path, file[:type].to_s, 'rpc_source', file[:name])
      File.open(path, 'w') { |io| io.write(file[:text].join) }
      file_report.puts path
    end

    file_report.close

    return true
  end
end

RpcGenerator.run(ARGV)

