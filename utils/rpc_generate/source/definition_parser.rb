
class Definition
  attr_accessor :name
  attr_accessor :type
  attr_accessor :parent
  attr_accessor :children
  attr_accessor :body
  attr_reader :position

  def initialize(attrs)
    @name = nil
    @type = nil
    @parent = nil
    @children = []
    @body = nil
    @position = attrs[:position]
    parse_attrs(attrs[:start])
  end

  def parse_attrs(start)
    tokens = start.split(/\b/)
    tokens.each_index {|index| tokens[index].strip! }
    tokens.delete_if {|token| token.empty? or token =~ /[^\w_]/ }

    @type = tokens[0]
    @name = tokens[1] if tokens.count > 1
    @name = 'anon' unless @name
  end

  def to_lineage_s
    lineage = @name
    node = @parent
    while node
      lineage = "#{node.name}::#{lineage}"
      node = node.parent
    end
    lineage
  end
end

class DefinitionParser
  DEF_START_REGEX = /\b[\w_]+[^\{\}]*\{/m

  def self.parse(filename)
    definitions = []

    text = File.read(filename)

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

    # Associated each open bracket with type specifier annotations.
    # Expected syntax is: type [name] {
    # The name parameter and spaces are optional.
    last_start = 0
    brackets.each do |bracket|
      if bracket[:type].eql? :open
        def_start = text.index(/(\bstruct|\bclass)/, last_start)
        text[def_start..(bracket[:position]+1)].scan(DEF_START_REGEX).each do |start|
          bracket[:start] = start
        end
      end
      last_start = bracket[:position]
    end


    definition_stack = []
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
        definition.body = text[definition.position..bracket[:position]].dup

        # Peek at the definition at the top of the stack.  The definition at
        # the top of the stack, if present, is this definition's parent.
        # Obviously, the current definition is also a child of its parent.
        definition.parent = definition_stack.last
        definition.parent.children << definition if definition.parent

        definitions << definition
      end
    end

    definitions
  end
end

