
class Structure

  attr_accessor :name
  attr_accessor :fields

  STRUCTURE = 'structure'
  REGEX = /\b#{STRUCTURE}\s+\w+\s*\{[\s\w\(\),]*\}/m

  @@templates = {:declaration_template => TextTemplate.new('templates/object_h.template'),
                 :definition_template => TextTemplate.new('templates/object_cpp.template')}

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

  def declaration_refs
    decl_list = Array.new
    @fields.each { |field| decl_list << field.to_decl(Field::REFERENCE) }
    decl_list
  end

  def unresolved_fields
    unresolved = Array.new
    @fields.each do |field|
      unresolved << field unless field.valid_type
    end
    unresolved
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

  def declaration
    fields = Hash.new
    fields['STRUCTURE_NAME'] = @name

    compiler_guard = Array.new
    @name.scan(/[A-Z][a-z0-9]+/).each {|token| compiler_guard << token.upcase }
    fields['COMPILER_GUARD'] = compiler_guard.join('_')

    fields['PARAMS'] = declarations.join(', ')
    fields['REF_PARAMS'] = declaration_refs.join(', ')

    {:name => "#{@name}.h",
     :text => @@templates[:declaration_template].build(fields)}
  end

  def definition
    fields = Hash.new
    fields['STRUCTURE_NAME'] = @name

    fields['PARAMS'] = declarations.join(', ')
    fields['REF_PARAMS'] = declaration_refs.join(', ')
    fields['PARAM_NAME'] = names

    {:name => "#{@name}.cpp",
     :text => @@templates[:definition_template].build(fields)}
  end
end

