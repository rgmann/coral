
module BlockParser

  class Block
    attr_accessor :name
    attr_accessor :type
    attr_accessor :parent
    attr_accessor :children
    attr_accessor :raw_text
    attr_accessor :text
    attr_reader :position

    attr_accessor :anonymous_count
    attr_accessor :is_anonymous
    attr_accessor :allow_anonymous

    def initialize(attrs={allow_anonymous: true})
      @name = nil
      @type = nil
      @parent = nil
      @children = []

      # Raw body text as parsed from block file.
      @raw_text = nil

      # Body text where child blocks in @raw_text have been
      # replaced with the child's name.
      @text = nil

      @is_anonymous = false
      @anonymous_count = 0
      @begin_position = 0
      @end_position = 0

      @allow_anonymous = attrs[:allow_anonymous]
    end

    def self.word_tokens(text)
      tokens = text.split(/\b/)
      tokens.each_index {|index| tokens[index].strip! }
      tokens.delete_if {|token| token.empty? or token =~ /[^\w_]/ }
      tokens
    end

    def open(bracket_attrs)
      @begin_position = bracket_attrs[:position]
      # By convention, the defintion begins at the start of the text preceding
      # the opening bracket, not the open bracket itself. Recompute the
      # block's start position.
      @begin_position -= bracket_attrs[:start].length

      tokens = Block.word_tokens(bracket_attrs[:start])

      @type = tokens[0]
      @name = tokens[1] if tokens.count > 1
      unless @name
        if @allow_anonymous
          @is_anonymous = true
          @name = 'anon'
        else
          throw "Anonymous definitions are not permitted."
        end
      end

      puts "open_tokens = #{tokens.inspect} #{@name}" if DEBUG
      true
    end

    def close(text, end_position)
      @end_position = end_position
      @raw_text = text[@begin_position..@end_position].dup

      # Child block pattern
      cbp = {true => /[^;]*;/, false => /;?/}

      # Replace child blocks with names. After replacements have
      # been completed, the inner_text should no longer have brackets.
      @text = @raw_text.dup
      @children.each do |child|
        @text.sub!(/#{Regexp.escape(child.raw_text)}#{cbp[@allow_anonymous]}/m) do |match|
          replacement = ''

          if @allow_anonymous
            # Replace the block with its name
            match.sub!(/#{Regexp.escape(child.raw_text)}/m, child.name)

            # Split the resulting string into cleaned word tokens.
            tokens = Block.word_tokens(match)

            # If there are two tokens, this block is immediately used
            # to specify the type for a member and we need to substitute.
            # Otherwise, this is a stand-alone block.
            replacement = (tokens.count < 2) ? "" : match
          end

          replacement
        end
      end

      @inner_text = Bracket.inner_text(@text, :curly)

      if DEBUG
        puts "#{mangle} allow_anonymous=#{@allow_anonymous.to_s}:"
        puts "  raw_text = #{@raw_text}"
        puts "  text = #{@text}"
        puts "  inner_text: #{@inner_text.inspect}"
        puts "\n\n"
      end
    end

    def add_child(child)
      child.parent = self
      child.name = next_anonymous_id.to_s if child.is_anonymous
      @children << child
    end

    def next_anonymous_id
      @anonymous_count += 1
      @anonymous_count
    end

    def to_lineage_s(sep="::")
      lineage = @name.dup
      node = @parent
      while node
        lineage = "#{node.name}#{sep}#{lineage}"
        node = node.parent
      end
      lineage
    end

    # Get the block's mangled name, which encodes name, type, parent type,
    # and depth.
    def mangle
      mname = to_lineage_s('$')
      mname << ".#{@type}"
      mname
    end

    # Computes the the depth of this node. If the node has no parent, its
    # depth is zero.
    def depth
      depth = 0
      depth = parent.depth + 1 if @parent
      depth
    end

    def satisfies_filter(filter)
      satisfied = true

      filter.each do |key, value|
        case key
        when :type
          satisfied = (@type ==  filter[:type])
        when :name
          satisfied = (@name == filter[:name])
        when :depth
          satisfied = (depth == filter[:depth])
        else
        end

        break unless satisfied
      end

      satisfied
    end

    def members(filter=nil)
      #@inner_text.scan(/\b[^;]*;/m) do |member|
      @inner_text.split(';').each do |member|
        # Remove the trailing semicolon (if present)
        member = member[/[^;]*/m]
        member.strip!

        # Assume a match in case a filter is not supplied.
        is_match = (member.empty? == false)
        is_match = (member =~ filter) if filter
        yield member if is_match
      end
    end
  end


  def self.remove_comments(text)
    text.gsub(/(#|\/\/)[^\n]*\n/, "\n")
  end

  module Bracket

    BRACKETS = {square:  [/\[[^\[\]]*\]/m, /[\[\]]/m, '[', ']'],
                paren:   [/\([^\(\)]*\)/m, /[\(\)]/m, '(', ')'],
                curly:   [/\{[^\{\}]*\}/m, /[\{\}]/m, '{', '}'],
                chevron: [/\<[^\<\>]*\>/m, /[\<\>]/m, '<', '>'],
                single_quote: [/\'[^\']*\'/m, /[\']/m, "'", "'"],
                double_quote: [/\"[^\"]*\"/m, /[\"]/m, "\"", "\""]}

    PATTERN_TYPES = [:match, :split, :open, :close]
                     
                     

    def self.patterns(type)
      patterns = []
      BRACKETS.each do |key, value|
        patterns << value[PATTERN_TYPES.index type]
      end
      patterns
    end

    def self.inner_text(text, bracket_type)
      unless BRACKETS.keys.include? bracket_type
        raise "Unknown bracket type: #{bracket_type}"
      end

      #puts "partition = #{text.partition(BRACKETS[bracket_type][PATTERN_TYPES.index :match])}"

      inner_text = text
      if text =~ BRACKETS[bracket_type][PATTERN_TYPES.index :match]
        inner_text = text.partition(BRACKETS[bracket_type][PATTERN_TYPES.index :match])[1]
        inner_text = inner_text.split(BRACKETS[bracket_type][PATTERN_TYPES.index :split])
        inner_text.delete_if { |token| token.empty? or (token =~ /\S/m).nil? } 
        inner_text = inner_text.first
        inner_text = '' unless inner_text 
      end
      inner_text
    end
  end

  class Parser
    attr_accessor :allow_anonymous

    def initialize

      # If anonymous definitions are allowed, syntax is expected to follow
      # strict C-like conventions. That is, blocks must end with a semicolon.
      #
      # If anonymous definitions are not allowed (protobuf IDL style), then
      # a definition can never anonymously as the type for another field.
      # All definitions must supply a name and members of that type must
      # specify the type explicitly by name.
      # 
      # Anonymous definitions and non-strict semicolon requirements cannot
      # be supported at the same time.
      @allow_anonymous = false
    end
  end

  def self.parse(filename, attributes={})
    collection = Collection.new

    text = File.read(filename)
    text = remove_comments(text)

    # Perform a quick test to verify that brackets balance.
    unless (text.count('{') == text.count('}'))
      raise "Brackets are not balanced."
    end

    # Scan the document for opening and closing brackets. Save each along
    # with its position and a tag indicating whether it is an opening or
    # closing bracket.
    pos = 0
    brackets = []
    while pos < text.length
      index = text.index(/[\{\}]/, pos)
      break unless index
      brackets << {:position => index,
                   :type => (text[index].eql? '{') ? :open : :close}
      pos = index + 1
    end

    # Match each opening bracket with all text that lies between it and a
    # preceding open bracket, closing bracket, or semicolon.  The 
    # block will clean up and parse this text itself.
    last_start = -1
    brackets.each do |bracket|
      if bracket[:type].eql? :open
        segment_of_interest = text[(last_start + 1)..(bracket[:position] - 1)]
        def_start = segment_of_interest.rindex /[;\{\}]/
        def_start = -1 unless def_start

        segment_of_interest = segment_of_interest[(def_start + 1)..-1]
        # Additionally, leave the white space the while (especially newline) behind.
        segment_of_interest = segment_of_interest[segment_of_interest.index(/[^\s]/)..-1]
        bracket[:start] = segment_of_interest
      end
      last_start = bracket[:position]
    end


    # Setup the root block.
    block_stack = [Block.new(allow_anonymous: attributes[:allow_anonymous])]
    block_stack.first.name = "global"
    block_stack.first.type = "global"

    brackets.each do |bracket|
      if bracket[:type].eql? :open
        block = Block.new(allow_anonymous: attributes[:allow_anonymous])
        block_stack.push(block) if block.open(bracket)
      else
        # The block at the top of the stack is closed first.  If this
        # pop returns nil, then the brackets are not balanced.
        block = block_stack.pop
        unless block
          raise "No matching open bracket for closing bracket at #{bracket[:position]}."
        end

        # Set the blocks body text.
        block.close(text, bracket[:position])

        # Peek at the block at the top of the stack.  The block at
        # the top of the stack, if present, is this block's parent.
        # Obviously, the current block is also a child of its parent.
        block_stack.last.add_child(block) if block_stack.last

        collection.blocks << block
      end
    end

    # Close the root block.
    block_stack.last.close(text, text.length - 1)
    collection.blocks << block_stack.pop

    collection
  end

  # Container to maintain a collection of blocks.
  class Collection

    attr_accessor :blocks

    def initialize
      @blocks = []
    end

    # Select all blocks that fulfill a filter. The filter may be empty,
    # to select all blocks, or the caller may supply multiple criteria.
    def select(filter={})
      @blocks.each do |block|
        if block.satisfies_filter(filter)
          yield block
        end
      end
    end
  end

  DEBUG = nil

end # module BlockParser 

