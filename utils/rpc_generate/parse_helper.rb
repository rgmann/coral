
module ParseHelper

  BLOCK_REGEX = /\b\w+\s+[\w_]+\s*\{[=;\s\w\_\&\(\),]*\}\s*/m

  def ParseHelper.blocks(text, data_structure = nil)
    text.scan(BLOCK_REGEX).each do |block|
      tokens = block.match(/\b\w+\s+[\w_]+\s*{/m).to_s.split(/[\s{]/)
      raise "Failed to parse data type and/or name." if tokens.count != 2

      data_type = tokens[0].strip
      name = tokens[1].strip
      body = ParseHelper.inside(block, "{", "}", /[=;\s\w\_\&\(\),]/m)

      yield data_type, name, body
    end
  end

  def ParseHelper.lines(text, pattern)
    unless pattern.class.to_s.eql? "Regexp"
      raise "Pattern should be of type Regexp"
    end

    text.scan(pattern).each do |line|
      yield line
    end
  end

  def ParseHelper.inside (text, dl, dr, allowed = /[,\/\w\s\.]/)
    inner = text.match(/#{dl}#{allowed}+#{dr}/m)
    inner = inner.to_s
    return nil unless inner

    inner = inner.scan(/#{dl}([^#{dr}]+)#{dr}/m).last
    if inner
      inner = inner.first
    else
      inner = ""
    end

    return inner
  end

  def ParseHelper.pairs (text, sepc = '=')
    text.scan(/\b[\w_]+\s*#{sepc}\s*\b[\w_]+/).each do |pair|
      name, sep, value = pair.partition(sepc)
      yield name.strip, value.strip
    end
  end
end


