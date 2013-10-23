
class RpcStruct

  attr_accessor :name
  attr_accessor :fields

  STRUCTURE = 'structure'
  REGEX = /\b#{STRUCTURE}\s+\w+\s*\{[\s\w\(\),]*\}/m

  def initialize(params = {})
    @name = params[:name]
    @fields = Array.new
    @nextParamPos = 0

    parse(params[:text]) if params[:text]
  end

  def from(text)
    
    # Try 
  end

  def << (param)
    field = Field.new
    type = param.match(/^\w+/)
    name = param.match(/\s+[\w_]+/)
    field.type = type.to_s.strip if type
    field.name = name.to_s.strip if name
    return unless type and name
    return unless Field::valid_type(field.type)
    field.index = @nextParamPos
    @fields << field
    @nextParamPos += 1
  end

  def to_s
    puts "struct #{@name}"
    @fields.each { |field| puts "  #{field.to_s}" }
  end

  def names
    name_list = Array.new
    @fields.each { |field| name_list << field.name }
    name_list
  end

  def declarations
    decl_list = Array.new
    @fields.each { |field| decl_list << field.to_decl }
    decl_list
  end

  def parse(definition)
    @name = definition.match(/\b#{STRUCTURE}\s+\w+\s*\{/m).to_s.split(/[\s\{]/)[1].to_s
    fields = definition.to_s.match(/\{.*\}/m).to_s

    # Parse the body of the structure definition.
    fields = fields.scan(/\b\w+\s+\b\w+\s*[,\n]/)
    fields.each do |field|
      puts "field = #{field}"
      @fields << Field.new(:declaration => field)
    end
  end
end

