
class RpcParameterList

  attr_accessor :fields
  attr_accessor :name

  def initialize(name)
    @name = name
    @fields = Array.new
    @nextParamPos = 0
  end

  def << (param)
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

  def to_gpb_message
    message = []
    message << "message #{name} {"

    @fields.each_index do |index|
      field = "   optional "
      field << "#{@fields[index].gpb_type} "
      field << "#{@fields[index].name}  = #{index + 1}"
      field << ";"
      message << field
    end
    message << "}"

    return message.join("\n")
  end
end

