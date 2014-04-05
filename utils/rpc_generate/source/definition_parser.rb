
module DefinitionParser

  class Definition
    attr_accessor :name
    attr_accessor :type
    attr_accessor :parent
    attr_accessor :children
    attr_accessor :raw_text
    attr_accessor :text
    attr_reader :position

    attr_accessor :anonymous_count
    attr_accessor :anonymous

    def initialize(bracket_attrs=nil)
      @name = nil
      @type = nil
      @parent = nil
      @children = []

      # Raw body text as parsed from definition file.
      @raw_text = nil

      # Body text where child definitions in @raw_text have been
      # replaced with the child's name.
      @text = nil

      @anonymous = false
      @anonymous_count = 0
      @begin_position = 0
      @end_position = 0

      if bracket_attrs
        open(bracket_attrs)
      end
    end

    def self.word_tokens(text)
      tokens = text.split(/\b/)
      tokens.each_index {|index| tokens[index].strip! }
      tokens.delete_if {|token| token.empty? or token =~ /[^\w_]/ }
      tokens
    end

    def open(bracket_attrs)
      @begin_position = bracket_attrs[:position]
      #@begin_position -= (bracket_attrs[:start] =~ /\{/)
      @begin_position -= bracket_attrs[:start].length

      tokens = Definition.word_tokens(bracket_attrs[:start])

      @type = tokens[0]
      @name = tokens[1] if tokens.count > 1
      unless @name
        @anonymous = true
        @name = 'anon'
      end
#      puts "open_tokens = #{tokens.inspect} #{@name}"
    end

    def close(text, end_position)
      @end_position = end_position
      @raw_text = text[@begin_position..@end_position].dup

      # Replace child definitions with names. After replacements have
      # been completed, the inner_text should no longer have brackets.
      @text = @raw_text.dup
      @children.each do |child|
        @text.sub!(/#{Regexp.escape(child.raw_text)}[^;]*;/m) do |match|
          # Replace the definition with its name
          match.sub!(/#{Regexp.escape(child.raw_text)}/m, child.name)

          # Split the resulting string into cleaned word tokens.
          tokens = Definition.word_tokens(match)

          # If there are two tokens, this definition is immediately used
          # to specify the type for a member and we need to substitute.
          # Otherwise, this is a stand-alone definition.
          (tokens.count == 2) ? match : ""
        end
      end

#      puts "#{mangle}:"
#      puts "  raw_text = #{@raw_text}"
#      puts "  text = #{@text}"
      @inner_text = @text.partition(/\{[^\}]*\}/m)[1].split(/[\{\}]/m)
      @inner_text.delete_if { |token| token.empty? or (token =~ /\S/m).nil? } 
      @inner_text = @inner_text.first
      @inner_text = '' unless @inner_text
#      puts "  inner_text: #{@inner_text}"
#      puts "\n\n"
    end

    def add_child(child)
      child.parent = self
      child.name = next_anonymous_id.to_s if child.anonymous
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

    # Get the definition's mangled name, which encodes name, type, parent type,
    # and depth.
    def mangle
      mname = to_lineage_s('$')
      mname << ".#{@type}"
      mname
    end

    def satisfies_filter(filter)
      satisfied = true
      filter.each do |key, value|
        case key
        when :type
          satisfied = (@type ==  filter[:type])
        when :name
          satisfied = (@name == filter[:name])
        else
        end
      end
      satisfied
    end

    def members(filter=nil)
      @inner_text.scan(/\b[^;]*;/m) do |member|
        is_match = true
        is_match = (member =~ filter) if filter
        yield member if is_match
      end
    end
  end

  def self.parse(filename)
    enumerator_list = Collection.new

    text = File.read(filename)

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
    # definition will clean up and parse this text itself.
    last_start = -1
    brackets.each do |bracket|
      if bracket[:type].eql? :open
        segment_of_interest = text[(last_start + 1)..(bracket[:position] - 1)]
        def_start = segment_of_interest.rindex /[;\{\}]/
        def_start = -1 unless def_start
        #bracket[:start] = segment_of_interest[(def_start + 1)..-1]
        segment_of_interest = segment_of_interest[(def_start + 1)..-1]
        # Additionally, leave the white space the while (especially newline) behind.
        segment_of_interest = segment_of_interest[segment_of_interest.index(/[^\s]/)..-1]
        bracket[:start] = segment_of_interest
      end
      last_start = bracket[:position]
    end


    # Setup the root definition.
    definition_stack = [Definition.new]
    definition_stack.first.name = "global"
    definition_stack.first.type = "global"

    brackets.each do |bracket|
      if bracket[:type].eql? :open
        definition_stack.push Definition.new(bracket)
      else
        # The definition at the top of the stack is closed first.  If this
        # pop returns nil, then the brackets are not balanced.
        definition = definition_stack.pop
        unless definition
          raise "No matching open bracket for closing bracket at #{bracket[:position]}."
        end

        # Set the definitions body text.
        definition.close(text, bracket[:position])

        # Peek at the definition at the top of the stack.  The definition at
        # the top of the stack, if present, is this definition's parent.
        # Obviously, the current definition is also a child of its parent.
        definition_stack.last.add_child(definition) if definition_stack.last

        enumerator_list.definitions << definition
      end
    end

    enumerator_list
  end


  class Collection

    attr_accessor :definitions

    def initialize
      @definitions = []
    end

    def select(filter={})
      @definitions.each do |definition|
        if definition.satisfies_filter(filter)
          yield definition
        end
      end
    end
  end

end # module DefinitionParser 

