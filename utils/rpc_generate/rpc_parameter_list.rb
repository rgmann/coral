
class RpcParameterList

  attr_accessor :fields

  @@templates = {:declaration_template =>
                 TextTemplate.new('templates/parameter_list_h.template'),
                 :definition_template =>
                 TextTemplate.new('templates/parameter_list_cpp.template'),
                 :marshall_template =>
                 TextTemplate.new('templates/parameter_list_marshall_cpp.template'),
                 :return_template =>
                 TextTemplate.new('templates/parameter_list_return_cpp.template')}

  def initialize(resource_name, action_name)
    @resource_name = resource_name
    @action_name = action_name
    @fields = Array.new
    @nextParamPos = 0
    @return_type = nil
  end

  def << (param)
    field = param
    return unless field.valid?
    field.index = @nextParamPos
    @fields << field
    @nextParamPos += 1
  end

  def return_type(type)
    @return_type = type
  end

  def name
    name = "#{@resource_name.capitalize}"
    name << "#{@action_name.capitalize}"
    name << "Input" unless @return_type
    name << "Output" if @return_type
    name
  end

  def names
    name_list = Array.new
    @fields.each { |field| name_list << field.name }
    name_list
  end

  def declarations
    decl_list = []
    @fields.each { |field| decl_list << field.declaration }
    decl_list
  end

  # Get the paramters as they would be specified in a C++ function
  # header. The string is returned as the single element of an
  # array.  An empty array is returned if there are no parameters.
  def parameter_list
    param_list = ''
    param_list << declarations.join(', ') if declarations.count > 0
    param_list
  end

  def references
    decl_list = []
    @fields.each do |field|
      decl_list << field.declaration(:type => :ref)
    end
    decl_list
  end

  # Get the parameters as a they would specified in a C++ function
  # header where all parameters are non-const reference parameters.
  # The string is returned as the single element of an array. An
  # empty array is returned if there are not parameters.
  def ref_parameter_list
    param_list = ''
    param_list << references.join(', ') if references.count > 0
    param_list
  end

  def name_value_pairs
    pairs = []
    names.each { |name| pairs << "\"#{name}\", #{name}" }
    pairs
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

  def reference_fields
    references = []
    @fields.each {|field| references << field if field.is_ref }
    references
  end

  def declaration
    fields = Hash.new
    fields['PARAMETER_LIST_NAME'] = name

    compiler_guard = Array.new
    name.scan(/[A-Z][a-z0-9]+/).each {|token| compiler_guard << token.upcase }
    fields['COMPILER_GUARD'] = compiler_guard.join('_')

    # It is assumed that all non standard types refer to classes extending
    # Structure.  Furthermore, It is assumed that each of these classes is
    # declared by itself in a file of the same name.  Therefore, build an
    # include list for non-standard types.
    fields['INCLUDES'] = unresolved_fields("#include \"%s.h\"")
    fields['INCLUDES'] = [] if fields['INCLUDES'].empty?

    fields['PARAMS'] = parameter_list
    fields['PARAMS'] = [] if fields['PARAMS'].empty?

    fields['REF_PARAMS'] = ref_parameter_list
    fields['REF_PARAMS'] = [] if fields['REF_PARAMS'].empty?

    fields['RETURN_TYPE'] = [@return_type]
    fields['RETURN_TYPE'].compact!

    {:name => "#{name}.h",
     :text => @@templates[:declaration_template].build(fields)}
  end

  def definition
    fields = Hash.new
    fields['PARAMETER_LIST_NAME'] = name

    fields['PARAMS'] = parameter_list
    fields['REF_PARAMS'] = ref_parameter_list
    fields['PARAM_NAMES'] = names

    fields['RETURN_VALUE_NAME'] = []
    fields['RETURN_VALUE_NAME'] = 'RpcReturnValue' if @return_type

    fields['PARAM_NAME_VALUES'] = name_value_pairs
    fields['RETURN_TYPE'] = @return_type

    fields['MARSHALL_ACCESSORS'] = []
    if not fields['PARAMS'].empty?
      fields['MARSHALL_ACCESSORS'] = @@templates[:marshall_template].build(fields).join
    end

    fields['RETURN_VALUE_ACCESSORS'] = []
    if @return_type
      fields['RETURN_VALUE_ACCESSORS'] = @@templates[:return_template].build(fields).join
    end

    {:name => "#{name}.cpp",
     :text => @@templates[:definition_template].build(fields)}
  end
end

