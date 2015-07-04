require 'securerandom'
require File.join(File.dirname(__FILE__), 'tokenizer')

module BlockParser

  class Symbol
    attr_reader :type
    attr_reader :name
    def initialize(type, name, value = nil)
      @type = type
    end

    def alias?
      (@type == :alias)
    end

    def target
      target = nil
      target = @value if alias?
      target
    end
  end

  class SymbolTable
    def initialize
      # Maps symbol name to symbol information.
      @symbols = {}
    end

    def resolve_symbol(symbol, path = [])
      symbol_info = @symbols[symbol]

      if symbol_info and symbol_info.alias?

        if path.include? symbol 
          throw "Circular alias detected on symbol: #{symbol}"

        else
          symbol_info = resolve_symbol(symbol_info.target, path << symbol)
        end

      end

      symbol_info
    end
  end

  class Collection
    attr_accessor :global
    def initialize
      @blocks = {}
      @global = nil
    end

    def blocks(&block)
      if block_given?
        @blocks.each do |uuid, block|
          yield uuid, block
        end
      end
      @blocks
    end

    def << (blocks)
      @blocks.merge!(blocks)
    end

    def resolve
      @blocks.each do |id, block|
        block.resolve_children(@blocks)
      end
      @global.resolve_children(@blocks)
    end
  end

  class Comment

    attr_reader :text

    def initialize(text, type)
      @type = type

      @text = ''
      if @type == :block
        @text = text.scan(/\/\*(.*)\*\//m).first.first
      elsif @type == :line
        @text = text.scan(/\/\/(.*)$/).first.first
      else
        throw "Invalid comment type: #{type}"
      end

      @uuid = nil
    end

    def uuid
      @uuid = SecureRandom::uuid unless @uuid
      @uuid
    end

    def << (other)
      if other.class == String
        @text << other

      elsif other.class == BlockParser::Comment
        @text << other.text

      else
        throw "Cannot append object of type #{other.class.to_s} to #{self.class.to_s}"

      end

      self
    end

    def self.parse(text)
      comments = []

      # Find and replace all block comments.
      text.gsub!(/\/\*.*\*\//m) do |match|
        comments << Comment.new(match, :block)
        " %#{comments.last.uuid}%;"
      end

      text.gsub!(/\/\/.*$/) do |match|
        comments << Comment.new(match || '', :line)
        " %#{comments.last.uuid}%;"
      end

      comments
    end
  end

  class Block
    def initialize(text, statement_delimeter = ';')
      @type = :anonymous

      @text = text
      @statement_delimeter = statement_delimeter

      inner_text = nil

      # Remove the enclosing braces (if any).
      @text.scan(/{([^}]*)}/m) { |match| inner_text = match.first }
      @text = inner_text if inner_text

      # Extract comments (if any)
      @comments = Comment::parse(@text)

      @uuid = nil
      @children = {}
    end

    def uuid
      @uuid = SecureRandom::uuid unless @uuid
      @uuid
    end

    def self.parse(text)
      blocks = {}
      text.gsub!(/\{[^\{\}]*\}/m) do |match|
        block = Block.new(match)
        blocks[block.uuid] = block
        " %#{block.uuid}% "
      end
      blocks
    end

    def statements
      @text.split(@statement_delimeter)
    end

    def resolve_children(block_table)
      @text.scan(/%\w{7}-\w{4}-\w{4}-\w{4}-\w{12}%/) do |id_token|
        match.match(/%(\w{7}-\w{4}-\w{4}-\w{4}-\w{12})%/) do |id|
          if block_table.keys.include? id
            @children[id.first] = block_table[id.first]
          else
            puts "Block::resolve_children: Failed to find block matching ID=#{id}"
          end
        end
      end
    end
  end

  class Parser
    def initialize
    end

    def self.parse(filename, &block)

      text = File.read(filename)
      collection = Collection.new

      # Perform a quick test to verify that brackets balance.
      unless (text.count('{') == text.count('}'))
        throw "Brackets are not balanced."
      end

      loop do
        blocks = Block::parse(text)
        break if blocks.empty?

        collection << blocks
      end

      unless text.empty?
        collection.global = Block.new(text)
      end

      collection.resolve
      collection
    end
  end

end

collection = BlockParser::Parser::parse(ARGV[0])
puts "GLOBAL"

File.open("#{File.basename(ARGV[0], '.h')}_out.h", 'w+') do |io|
  collection.blocks do |uuid, block|
    io.puts "UUID=#{uuid}"
    block.statements.each do |statement|
      io.puts "  STATEMENT:"
      io.puts "  #{statement.strip}"
#      io.puts "TOKENS:"
#      io.puts Tokenizer::tokens(statement).inspect
    end
    io.puts "\n\n"
  end
end
#puts collection.global.statements.inspect

