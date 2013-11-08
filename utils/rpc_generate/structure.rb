
class Structure

  attr_accessor :name
  attr_accessor :fields

  STRUCTURE = 'structure'
  REGEX = /\b#{STRUCTURE}\s+\w+\s*\{[\s\w\(\);]*\}/m

  @@templates = {:declaration_template => TextTemplate.new('templates/object_h.template'),
                 :definition_template => TextTemplate.new('templates/object_cpp.template')}

  def initialize(params = {})
    @name = params[:name]
#    puts "name = #{@name}"
    @fields = Array.new
    @nextParamPos = 0

    parse(params[:text]) if params[:text]
  end

  def << (param)
    #puts "Structure << #{param}"
    #field = Field.new(:declaration => param)
    field = param
    return unless field.valid?
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
    @fields.each { |field| decl_list << field.declaration }
    decl_list
  end

  def declaration_refs
    decl_list = Array.new
    @fields.each { |field| decl_list << field.declaration(:type => :ref) }
    decl_list
  end

  def unresolved_fields(fmt_str = nil)
    unresolved = Array.new
    @fields.each do |field|
      unless field.valid_type
        unresolved << field if fmt_str.nil?
        unresolved << fmt_str % field.type if fmt_str
      end
    end
    unresolved
  end

  def ref_fields
    references = Array.new
    @fields.each {|field| references << field if field.is_ref }
    references
  end

  def parse(definition)
    @name = definition.match(/\b#{STRUCTURE}\s+\w+\s*\{/m).to_s.split(/[\s\{]/)[1].to_s
    fields = definition.to_s.match(/\{.*\}/m).to_s

    # Parse the body of the structure definition.
    fields = fields.scan(/\b\w+\s+\b\w+\s*[;\n]/)
    fields.each do |raw_field|
      field = Field.new
      @fields << field if field.parse(raw_field)
    end
  end

  def declaration
    fields = Hash.new
    fields['STRUCTURE_NAME'] = @name

    compiler_guard = Array.new
    @name.scan(/[A-Z][a-z0-9]+/).each {|token| compiler_guard << token.upcase }
    fields['COMPILER_GUARD'] = compiler_guard.join('_')

    # It is assumed that all non standard types refer to classes extending
    # Structure.  Furthermore, It is assumed that each of these classes is
    # declared by itself in a file of the same name.  Therefore, build an
    # include list for non-standard types.
    fields['INCLUDES'] = unresolved_fields("#include \"%s.h\"")
    fields['INCLUDES'] = '' if fields['INCLUDES'].empty?

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

