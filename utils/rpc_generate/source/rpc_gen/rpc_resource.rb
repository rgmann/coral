require File.join(File.dirname(__FILE__), 'option_hash')
require File.join(File.dirname(__FILE__), 'rpc_resource_action')

class RpcResource

  attr_accessor :name
  attr_accessor :actions

  @@namespace = nil

  def initialize(template_collection, name, body)
    @actions = Array.new
    @options = OptionHash.new
    @ctor_param_list = Array.new
    @name = name

    @options.add(:allow_exceptions, false, [true, false], lambda { |value|
      cval = nil
      cval = false if ['false', 'no'].include? value
      cval = true if ['true', 'yes'].include? value
      cval
    })

    @options.add(:static, false, [true, false], lambda { |value|
      cval = nil
      cval = false if ['false', 'no'].include? value
      cval = true if ['true', 'yes'].include? value
      cval
    })

    initialize_templates(template_collection)

    parse(body)

  end

  def self.set_namespace(namespace)
    @@namespace = namespace
    RpcResourceAction.set_namespace(namespace)
  end

  def initialize_templates(template_collection)
    @template_collection = template_collection
    @template_collection.add_common_field('nspace', @@namespace)

    @template_collection.add_template(:server_stub_decl,
                                      "templates/server_resource_stub_h.template",
                                      :server,
                                      "%sServerStub.h")
    @template_collection.add_template(:server_stub_def,
                                      "templates/server_resource_stub_cpp.template",
                                      :server,
                                      "%sServerStub.cpp")
    @template_collection.add_template(:inst_wrapper_decl,
                                      "templates/instance_wrapper_h.template",
                                      :server,
                                      "%sWrapper.h")
    @template_collection.add_template(:inst_wrapper_def,
                                      "templates/instance_wrapper_cpp.template",
                                      :server,
                                      "%sWrapper.cpp")
    @template_collection.add_template(:inst_wrapper_user_def,
                                      'templates/instance_wrapper_user_cpp.template',
                                      :server,
                                      "user.%sWrapper.cpp")
    @template_collection.add_template(:client_stub_decl,
                                      "templates/client_stub_resource_h.template",
                                      :client,
                                      "%sClientStub.h")
    @template_collection.add_template(:client_stub_def,
                                      "templates/client_stub_resource_cpp.template",
                                      :client, 
                                      "%sClientStub.cpp")
  end

  def create_descriptor(name, fields)
    @template_collection.add_common_field('wrapped_class', @name)
    @template_collection.add_common_field('comp_guard', @name.upcase)
    @template_collection.create_descriptor(name, fields, @name)
  end

  def parse(body)
    ParseHelper.lines(body, /\b[\w_\.]+\s+\b[\w_]+\s*\(.*\)/) do |line|
      request = ParseHelper.inside(line, Regexp.escape("("), Regexp.escape(")"), /[\._\w\s]/)
      #param_list = param_list.split(',')
      #param_list.each { |param| param.strip! }

      type_name = line.match(/\b[\w_]+\s+\b[\w_]+\s*\(/)
      response, action_name = type_name.to_s.split(/[\s+\(]/)
      @actions << RpcResourceAction.new(@template_collection, @name,
                                        action_name, request, response)
    end
    ParseHelper.pairs(body) do |name, value|
      @options[name] = value if @options.include? name
    end
    ParseHelper.lines(body, /^\s*\b[\w_]+\s*;/) do |line|
      name = line.match(/\s*\b[\w_]+\s*/)
      name = name.to_s.strip if name
      next unless name
      @options[name] = true if @options.include?(name)
    end
  end

  def to_wrapper_decl
    fields = Hash.new
    
    delegates = Array.new
    wrappers = Array.new
    @actions.each do |action|
      delegates << action.name
      wrappers << action.to_wrapper_action_decl
    end
    fields['delegates'] = delegates
    fields['actions'] = wrappers

    return create_descriptor(:inst_wrapper_decl, fields)
  end

  def to_wrapper_def
    fields = Hash.new

    actions = Array.new
    #action_plists = Array.new
    @actions.each do |action|
      actions << action.to_wrapper_action_def(fields).join
    #  action_plists.concat(action.param_list_names)
    end
    #fields['action_structs'] = action_plists
    fields['actions'] = actions

    return create_descriptor(:inst_wrapper_def, fields)
  end

  def to_wrapper_user_def
    fields = {}

    actions = []
    @actions.each do |action|
      actions << action.to_wrapper_user_action_def(fields).join
    end

    fields['actions'] = actions

    return create_descriptor(:inst_wrapper_user_def, fields)
  end

  def to_server_stub_decl
    fields = Hash.new
    return create_descriptor(:server_stub_decl, fields)
  end

  def to_server_stub_def
    fields = Hash.new

    actions = Array.new
    @actions.each do |action|
      actions << "\"#{action.name}\", #{@name}Wrapper::#{action.name}Delegate"
    end
    fields['actions'] = actions

    return create_descriptor(:server_stub_def, fields)
  end

  def to_client_stub_decl
    fields = Hash.new

    actions = Array.new
    @actions.each do |action|
      action.allow_exceptions = @options[:allow_exceptions]
      actions << action.to_client_stub_action
    end
    fields['actions'] = actions
    fields['includes'] = []
    fields['includes'] << "#include \"#{@@namespace}.pb.h\""

    return create_descriptor(:client_stub_decl, fields)
  end

  def to_client_stub_def
    fields = Hash.new

    actions = Array.new
    #action_plists = Array.new
    @actions.each do |action| 
      action.allow_exceptions = @options[:allow_exceptions]
      actions << action.to_client_stub_action_def(fields).join
      #action_plists.concat(action.param_list_names)
    end
    #fields['action_struct'] = action_plists
    fields['actions'] = actions

    return create_descriptor(:client_stub_def, fields)
  end

  def declarations
    includes = Array.new
    includes << to_wrapper_decl
    includes << to_server_stub_decl
    includes << to_client_stub_decl
    includes
  end

  def definitions
    source = Array.new
    source << to_wrapper_def
    source << to_wrapper_user_def
    source << to_client_stub_def
    source << to_server_stub_def
    source
  end

  def messages
    messages = []
    @actions.each { |action| messages.concat(action.messages) }
    messages
  end
end

