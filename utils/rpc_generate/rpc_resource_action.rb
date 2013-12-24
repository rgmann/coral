require File.join(File.dirname(__FILE__), "template_collection")

class RpcResourceAction

  attr_accessor :resource
  attr_accessor :name
  attr_accessor :request
  attr_accessor :response
  attr_accessor :allow_exceptions

  @@namespace = nil

  def initialize(template_collection, resource, name, request, response)
    @name = name
    @resource = resource
    @request = nil
    @response = nil
    @allow_exceptions = false

    validate_params(request, response)
    initialize_templates(template_collection)
  end

  def self.set_namespace (namespace)
    @@namespace = namespace
  end

  def self.set_message_list (message_list)
    @@message_list = message_list
  end

  def initialize_templates(template_collection)
    @template_collection = template_collection
    @template_collection.add_common_field('nspace', @@namespace)

    @template_collection.add_template(:inst_wrapper_action,
                                      "templates/instance_wrapper_generated_action_cpp.template")
    @template_collection.add_template(:inst_wrapper_user_action,
                                      "templates/instance_wrapper_user_action_cpp.template")
    @template_collection.add_template(:client_stub_action_ne,
                                      "templates/client_stub_action_cpp_ne.template")
    @template_collection.add_template(:client_stub_action,
                                      "templates/client_stub_action_cpp.template")
  end

  def validate_params(request, response)
    @request = @@message_list[request]
    unless @request
      @request = PbMessage.new(request)

      msg = "WARNING: "
      msg << "Namespace not set for request message \"#{@request.type}\" "
      msg << "(action: #{@resource}.#{@name})"
      puts msg
    end

    @response = @@message_list[response]
    unless @response
      @response = PbMessage.new(response)

      msg = "WARNING: "
      msg << "Namespace not set for response message \"#{@response.type}\" "
      msg << "(action: #{@resource}.#{@name})"
      puts msg
    end
  end

  def fill_template(name, fields)
    @template_collection.add_common_field('action_name', @name)
    @template_collection.add_common_field('wrapped_class', @resource)
    @template_collection.add_common_field('return_type', @return_type)
    @template_collection.add_common_field('in_message', @request.declaration)
    @template_collection.add_common_field('in_param', @request.declaration(:format => :const_reference))
    @template_collection.add_common_field('out_message', @response.declaration)
    @template_collection.add_common_field('out_param', @response.declaration(:format => :reference))

    return @template_collection.fill_template(name, fields)
  end

  def param_list_names
    [@request.name, @response.name]
  end

  def to_wrapper_action_decl
    params = []
    params << @request.declaration(:format => :const_reference, :name => 'request', :nspace => true)
    params << @response.declaration(:format => :reference, :name => 'response', :nspace => true)
    params << "liber::rpc::RpcException& e"
    decl = "void #{@name}("
    decl << params.join(', ')
    decl << ");"
    return decl
  end

  def to_wrapper_action_def (inherited)
    action_fields = Hash.new

#    action_fields['in_param_getters'] = []
#    @request.fields.each do |field|
#      action_fields['in_param_getters'] << "lInMessage.#{field.gpb_getter}"
#    end
#    if action_fields['in_param_getters'].empty?
#      action_fields['in_param_getters'] = ''
#    else
#      action_fields['in_param_getters'] = action_fields['in_param_getters'].join ', '
#    end

    fields = inherited.merge(action_fields)
    fill_template(:inst_wrapper_action, fields)
  end

  def to_wrapper_user_action_def (inherited)
    action_fields = {}
#    action_fields['in_param_list'] = @request.declarations.join(', ')
    fields = inherited.merge(action_fields)
    fill_template(:inst_wrapper_user_action, fields)
  end

  def to_client_stub_action
#    return_type = "bool"
#    return_type = @return_type if @allow_exceptions

    #param_list = @request.declarations
    #param_list << "#{@return_type}& result" if not @allow_exceptions

    #decl = "#{return_type} #{@name}(#{param_list.join(', ')})"
    params = []
    params << @request.declaration(:format => :const_reference, :name => 'request', :nspace => true)
    params << @response.declaration(:format => :reference, :name => 'response', :nspace => true)
    decl = "#{@name}(#{params.join(', ')})"
    decl << " throw (liber::rpc::RpcException)" if @allow_exceptions
    "#{decl};"
  end

  def to_client_stub_action_def (inherited)
    action_fields = Hash.new

#    action_fields['in_param_setters'] = []
#    @request.fields.each do |field|
#      action_fields['in_param_setters'] << "lInMessage.#{field.gpb_setter}"
#    end
#    if action_fields['in_param_setters'].empty?
#      action_fields['in_param_setters'] = ''
#    end

    fields = action_fields.merge(inherited)

    lines = []
    if @allow_exceptions
      lines = fill_template(:client_stub_action, fields)
    else
      lines = fill_template(:client_stub_action_ne, fields)
    end

    lines
  end

#  def messages
#    [@request.to_gpb_message, @response.to_gpb_message]
#  end
end

