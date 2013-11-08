
class RpcResourceAction

  attr_accessor :class_name
  attr_accessor :name
  attr_accessor :in_params
  attr_accessor :out_params
  attr_accessor :return_type
  attr_accessor :allow_exceptions

  @@templates = {:inst_wrapper_action => TextTemplate.new('templates/instance_wrapper_action_cpp.template'),
                 :inst_wrapper_user_action => TextTemplate.new('templates/instance_wrapper_user_action_cpp.template'),
                 :client_stub_action_ne => TextTemplate.new('templates/client_stub_action_cpp_ne.template'),
                 :client_stub_action => TextTemplate.new('templates/client_stub_action_cpp.template')}

  def initialize(wrapped_class, name, parameters, return_type)
    @name = name
    @return_type = return_type
    @class_name = wrapped_class
    @allow_exceptions = false

    param_list = Array.new
    param_list = parameters if parameters
    puts "param_list = #{param_list.inspect}"
    create_params("#{wrapped_class.capitalize}#{@name.capitalize}", param_list)
  end

  def create_params(prefix, raw_params)

    # Parse the list of fields.
    fields = []
    raw_params.each do |param|
      field = Field.new
      fields << field if field.parse(param)
    end

    # Create the input and output param lists
    @in_params = RpcParameterList.new(@class_name, @name)

    @out_params = RpcParameterList.new(@class_name, @name)
    @out_params.return_type(@return_type)
    
    fields.each do |field|
      @in_params << field.dup
      @out_params << field.dup if field.ref_type.eql? :ref
    end
  end

  def param_list_names
    [@in_params.name, @out_params.name]
  end

  def to_wrapper_action_decl
    "#{@return_type} #{@name}(#{@in_params.declarations.join(', ')});"
  end

  def to_wrapper_action_def(inherited)
    action_fields = Hash.new
    action_fields['ACTION_NAME'] = @name
    action_fields['RETURN_TYPE'] = @return_type

    action_fields['PARAM_DECLS'] = Array.new
    @in_params.fields.each do |field|
      action_fields['PARAM_DECLS'] << "#{field.declaration(:type => :value, :init => true)};"
    end

    action_fields['GET_IN_PARAMS'] = @in_params.names.join(', ')
    action_fields['ACTION_STRUCT_IN'] = @in_params.name

    action_fields['SET_OUT_PARAMS'] = []
    if @out_params.names.count > 0
      action_fields['SET_OUT_PARAMS'] << @out_params.names.join(', ')
    end

    action_fields['ACTION_STRUCT_OUT'] = @out_params.name

    action_fields['GET_IN_PARAM_LIST'] = @in_params.declarations.join(', ')

    fields = inherited.merge(action_fields)
    @@templates[:inst_wrapper_action].build(fields)
  end

  def to_wrapper_user_action_def(inherited)
    action_fields = {}
    action_fields['ACTION_NAME'] = @name
    action_fields['RETURN_TYPE'] = @return_type
    action_fields['GET_IN_PARAM_LIST'] = @in_params.declarations.join(', ')
    fields = inherited.merge(action_fields)
    @@templates[:inst_wrapper_user_action].build(fields)
  end

  def to_client_stub_action
    return_type = "bool"
    return_type = @return_type if @allow_exceptions

    param_list = @in_params.declarations
    param_list << "#{@return_type}& result" if not @allow_exceptions

    decl = "#{return_type} #{@name}(#{param_list.join(', ')})"
    decl = "#{decl} throw (RpcError)" if @allow_exceptions
    "#{decl};"
  end

  def to_client_stub_action_def(inherited)
    action_fields = Hash.new

    param_list = @in_params.declarations
    #param_list << "#{@return_type}& result" if not @allow_exceptions
    action_fields['PARAMS'] = param_list.join(', ')
    action_fields['RETURN_TYPE'] = @return_type

    action_fields['IN_PARAMS'] = ''
    if @in_params.names.count > 0
      action_fields['IN_PARAMS'] = "#{@in_params.names.join(', ')}"
    end

    action_fields['OUT_PARAMS'] = []
    if @out_params.names.count > 0
      action_fields['OUT_PARAMS'] << "#{@out_params.names.join(',')}"
    end

    action_fields['ACTION'] = @name
    action_fields['ACTION_PARAM_LIST_IN'] = @in_params.name
    action_fields['ACTION_PARAM_LIST_OUT'] = @out_params.name
    fields = action_fields.merge(inherited)

    lines = []
    if @allow_exceptions
      lines = @@templates[:client_stub_action].build(fields)
    else
      lines = @@templates[:client_stub_action_ne].build(fields)
    end
    lines
  end

  def to_action_param_list_decl
    [@in_params.declaration, @out_params.declaration]
  end

  def to_action_param_list_def
    [@in_params.definition, @out_params.definition]
  end
end

