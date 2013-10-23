#require File.join(File.expand_path('./'), 'text_template.rb')
#require File.join(File.expand_path('./'), 'rpc_struct.rb')

class RpcResourceAction

  attr_accessor :name
  attr_accessor :parameters
  attr_accessor :return_type

  @@templates = {:inst_wrapper_action => TextTemplate.new('templates/instance_wrapper_action_cpp.template'),
                 :client_stub_action_ne => TextTemplate.new('templates/client_stub_action_cpp_ne.template'),
                 :action_paramlist_decl => TextTemplate.new('templates/object_h.template'),
                 :action_paramlist_def => TextTemplate.new('templates/object_cpp.template')}

  def initialize(wrapped_class, name, parameters, return_type)
    @name = name
    @return_type = return_type

    object_name = "#{wrapped_class.capitalize}#{@name}ParamList"
    @parameters = RpcStruct.new(:name => object_name)
    param_list = Array.new
    param_list = parameters if parameters
    param_list.each do |param|
      @parameters << param
    end
  end

  def param_list_name
    @parameters.name
  end

  def to_wrapper_action_src
    "void #{@name}(#{ACTION_DEF_PARAMS});"
  end

  def to_wrapper_action_def(inherited)
    action_fields = Hash.new
    action_fields['ACTION_NAME'] = @name
    action_fields['RETURN_TYPE'] = @return_type

    param_decls = Array.new
    param_name_list = Array.new
    @parameters.fields.each do |parameter|
      param_decls << "#{parameter.to_decl};"
      param_name_list << parameter.name
    end
    action_fields['PARAM_DECLS'] = param_decls
    action_fields['GET_PARAMS'] = param_name_list.join(', ')
    action_fields['ACTION_STRUCT'] = @parameters.name

    fields = inherited.merge(action_fields)
    @@templates[:inst_wrapper_action].build(fields)
  end

  def to_client_stub_action
    param_list = Array.new
    @parameters.fields.each do |parameter|
      param_list << parameter.to_decl
    end
    param_list << "#{@return_type}& result"

    "bool #{@name}(#{param_list.join(', ')});"
  end

  def to_client_stub_action_def(inherited)
    action_fields = Hash.new

    param_list = Array.new
    param_name_list = Array.new
    @parameters.fields.each do |parameter|
      param_list << parameter.to_decl
      param_name_list << parameter.name
    end
    param_list << "#{@return_type}& result"

    action_fields['PARAMS'] = param_list.join(', ')

    action_fields['STRUCT_PARAMS'] = ''
    if param_name_list.count > 0
      action_fields['STRUCT_PARAMS'] = "(#{param_name_list.join(', ')})"
    end

    action_fields['ACTION'] = @name
    action_fields['ACTION_STRUCT'] = @parameters.name
    fields = action_fields.merge(inherited)

    @@templates[:client_stub_action_ne].build(fields)
  end

  def to_action_param_list_decl(wrapped_class)
    fields = Hash.new
    fields['WRAPPED_CLASS_ACTION'] = @parameters.name
    fields['COMP_GUARD_ACTION'] = "#{wrapped_class.upcase}_#{@name.upcase}"

    param_list = Array.new
    param_ref_list = Array.new
    @parameters.fields.each do |parameter|
      param_list << parameter.to_decl
      param_ref_list << "#{parameter.type}& #{parameter.name}"
    end
    fields['PARAMS'] = param_list.join(', ')
    fields['REF_PARAMS'] = param_ref_list.join(', ')

    {:name => "#{fields['WRAPPED_CLASS_ACTION']}ParamList.h",
     :text => @@templates[:action_paramlist_decl].build(fields)}
  end

  def to_action_param_list_def(wrapped_class)
    fields = Hash.new
    fields['WRAPPED_CLASS_ACTION'] = "#{wrapped_class}#{@name.capitalize}"

    param_list = Array.new
    param_ref_list = Array.new
    param_names = Array.new
    @parameters.fields.each do |parameter|
      param_list << parameter.to_decl
      param_ref_list << "#{parameter.type}& #{parameter.name}"
      param_names << parameter.name
    end
    fields['PARAMS'] = param_list.join(', ')
    fields['REF_PARAMS'] = param_ref_list.join(', ')
    fields['PARAM_NAME'] = param_names

    puts "apl = #{fields['WRAPPED_CLASS_ACTION']}"

    {:name => "#{fields['WRAPPED_CLASS_ACTION']}ParamList.cpp",
     :text => @@templates[:action_paramlist_def].build(fields)}
  end
end
