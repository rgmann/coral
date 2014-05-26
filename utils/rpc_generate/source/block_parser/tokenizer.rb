module Tokenizer

  module Bracket

    BRACKETS = {square:  [/\[[^\]]*\]/m, /[\[\]]/m, '[', ']'],
                paren:   [/\([^\)]*\)/m, /[\(\)]/m, '(', ')'],
                curly:   [/\{[^\{\}]*\}/m, /[\{\}]/m, '{', '}'],
                chevron: [/\<[^\>]*\>/m, /[\<\>]/m, '<', '>'],
                single_quote: [/\'[^\']*\'/m, /[\'\']/m, "'", "'"],
                double_quote: [/\"[^\"]*\"/m, /[\"\"]/m, "\"", "\""]}

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

      inner_text = text.partition(BRACKETS[bracket_type][PATTERN_TYPES.index :match])[1]
      inner_text = inner_text.split(BRACKETS[bracket_type][PATTERN_TYPES.index :split])
      inner_text.delete_if { |token| token.empty? or (token =~ /\S/m).nil? }
      inner_text = inner_text.first
      inner_text = '' unless inner_text
      inner_text
    end
  end

  # Splits a string into tokens and returns a hash where :bracket indicates
  # the type of bracket that enclosed the set of tokens and :tokens specifies
  # the set of tokens.  If recurse = true, the method will recursively tokenize
  # sub blocks.  Otherwise, sub blocks are not tokenized, but simply specified
  # as strings.
  def self.tokens(text)
    blocks = {}
    block_index = 0
    while true
      sub_count = 0
      Bracket::BRACKETS.each do |type, attributes|
        text.sub!(attributes[Bracket::PATTERN_TYPES.index :match]) do |block|
          block_name = "$block#{block_index}"
          blocks[block_name] = [type, Bracket.inner_text(block, type)]
          block_index += 1
          sub_count += 1
          " #{block_name} "
        end
        break if sub_count > 0
      end
      break if sub_count == 0
    end

    tokenize(text, blocks)
  end

  def self.tokenize(segment, block_table)
    tokens = segment.split(/(\s|;)/m) # /(\s|;)/m
    tokens.each_index {|index| tokens[index].strip! }
    tokens.delete_if {|token| token.empty? }
    tokens.each_index do |index|
      if block_table.keys.include? tokens[index]
        tokens[index] = [block_table[tokens[index]][0], tokenize(block_table[tokens[index]][1], block_table)]
      end
    end
    tokens
  end
end

