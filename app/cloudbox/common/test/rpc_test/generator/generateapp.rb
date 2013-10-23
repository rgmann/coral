#!/usr/bin/ruby

require File.join(File.expand_path('./'), 'text_template.rb')

class Field

  TYPES = ['i32',
           'i64',
           'Bool',
           'Double',
           'String',
           'Struct']

  PBTYPE = { :pass_by_reference => "pass_by_reference",
             :pass_by_value => "pass_by_value"}

  attr_accessor :type
  attr_accessor :index
  attr_accessor :name
  attr_accessor :default
  attr_accessor :pbtype

  def self.valid_type(type)
    TYPES.include?(type)
  end

  def to_s
    "name: #{@name}, type: #{@type}, index: #{@index}, p_type: #{PBTYPE[@pbtype]}"
  end

  def to_decl
    "#{@type} #{@name}"
  end
end

class RpcStruct

  attr_accessor :name
  attr_accessor :fields

  def initialize(name)
    @name = name
    @fields = Array.new
    @nextParamPos = 0
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
    #puts "#{field.name}, #{field.type}, #{Field::valid_type(type)}"
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

  def to_obect_def
  end
end

class RpcResourceAction

  attr_accessor :name
  attr_accessor :parameters
  attr_accessor :return_type

  @@templates = {:inst_wrapper_action => TextTemplate.new('templates/instance_wrapper_action_cpp.template'),
                 :client_stub_action_ne => TextTemplate.new('templates/client_stub_action_cpp_ne.template'),
                 :action_paramlist_decl => TextTemplate.new('templates/object_h.template'),
                 :action_paramlist_def => TextTemplate.new('templates/object_cpp.template')}

  def initialize(name, parameters, return_type)
    @name = name
    @return_type = return_type

    @parameters = RpcStruct.new("#{@name}_params")
    param_list = Array.new
    param_list = parameters if parameters
    param_list.each do |param|
      @parameters << param
    end
  end

  def to_s
    string = String.new
    string << "  action #{@name}:\n"
    string << "    return_type: #{@return_type}\n"
    string << "    struct param_list:\n"
    @parameters.fields.each { |field| string << "      #{field.to_s}\n" }
    string
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
    action_fields['ACTION_STRUCT'] = "#{inherited['WRAPPED_CLASS']}#{@name.capitalize}"

    fields = inherited.merge(action_fields)
    @@templates[:inst_wrapper_action].build(fields)
  end

  def to_client_stub_action
    param_list = Array.new
    @parameters.fields.each do |parameter|
      param_list << parameter.to_decl
    end
    param_list << "#{@return_type}& result"

    "void #{@name}(#{param_list.join(', ')});"
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
    action_fields['STRUCT_PARAMS'] = param_name_list.join(', ')
    action_fields['ACTION'] = @name
    action_fields['ACTION_STRUCT'] = "#{inherited['WRAPPED_CLASS']}#{@name.capitalize}"
    fields = action_fields.merge(inherited)

    @@templates[:client_stub_action_ne].build(fields)
  end

  def to_action_param_list_decl(wrapped_class)
    fields = Hash.new
    fields['WRAPPED_CLASS_ACTION'] = "#{wrapped_class}#{@name.capitalize}"
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

class RpcResource

  RESOURCE = 'resource'
  RESOURCE_REGEX = /\b#{RESOURCE}\s+\w+\s*\{[\s\w\(\),]*\}/m

  attr_accessor :name
  attr_accessor :actions

  @@templates = {:server_stub_decl => TextTemplate.new('templates/server_resource_stub_h.template'),
                 :inst_wrapper_decl => TextTemplate.new('templates/instance_wrapper_h.template'),
                 :inst_wrapper_def => TextTemplate.new('templates/instance_wrapper_cpp.template'),
                 :client_stub_decl => TextTemplate.new('templates/client_stub_resource_h.template'),
                 :client_stub_def => TextTemplate.new('templates/client_stub_resource_cpp.template')}

  def initialize
    @actions = Array.new
  end

  def parse(definition)
    @name = definition.match(/\b#{RESOURCE}\s+\w+\s*\{/m).to_s.split(/[\s\{]/)[1].to_s
    #actions = definition.to_s.scan(/\{(.*)\}/m).last.first.to_s
    actions = definition.to_s.match(/\{.*\}/m).to_s
    p actions

    # Parse the body of the resource definition.
    actions = actions.scan(/\b\w+\s+\b\w+\s*\(.*\)[,\n]/)
    actions.each do |action|
      #param_list = action.match(/\([\w,\s]*\)/).to_s
      param_list = action.scan(/\(([^\)]+)\)/).last
      p param_list
      if param_list
        param_list = param_list.first.split(',')
        param_list.each {|param| param.strip! }
      end
      type_name = action.match(/\b\w+\s+\b\w+\s*\(/)
      type, name = type_name.to_s.split(/[\s+\(]/)
      @actions << RpcResourceAction.new(name, param_list, type)
    end
  end

  def to_s
    string = String.new
    string << "resource #{name}:\n"
    @actions.each { |action| string << action.to_s }
    string
  end

  def to_wrapper_decl
    fields = Hash.new
    fields['COMP_GUARD'] = @name.upcase
    fields['WRAPPED_CLASS'] = @name
    
    actions = Array.new
    @actions.each do |action|
      actions << action.name
    end
    fields['ACTIONS'] = actions

    {:name => "#{@name}Wrapper.h",
     :text => @@templates[:inst_wrapper_decl].build(fields)}
  end

  def to_wrapper_def
    fields = Hash.new
    fields['WRAPPED_CLASS'] = @name

    actions = Array.new
    @actions.each do |action|
      actions << action.to_wrapper_action_def(fields).join
    end
    fields['ACTIONS'] = actions

    {:name => "#{@name}Wrapper.cpp",
     :text => @@templates[:inst_wrapper_def].build(fields)}
  end

  def to_server_stub_decl
    fields = Hash.new
    fields['COMP_GUARD'] = @name.upcase
    fields['WRAPPED_CLASS'] = @name

    actions = Array.new
    @actions.each do |action|
      actions << "      addAction(\"#{action.name}\", #{@name}Wrapper::#{action.name});"
    end
    fields['ACTIONS'] = actions

    {:name => "#{@name}ServerStub.h",
     :text => @@templates[:server_stub_decl].build(fields)}
  end

  def to_client_stub_decl
    fields = Hash.new
    fields['COMP_GUARD'] = @name.upcase
    fields['WRAPPED_CLASS'] = @name

    actions = Array.new
    @actions.each { |action| actions << action.to_client_stub_action }
    fields['ACTIONS'] = actions

    {:name => "#{@name}ClientStub.h",
     :text => @@templates[:client_stub_decl].build(fields)}
  end

  def to_client_stub_def
    fields = Hash.new
    fields['WRAPPED_CLASS'] = @name

    actions = Array.new
    @actions.each do |action| 
      actions << action.to_client_stub_action_def(fields).join
    end
    fields['ACTIONS'] = actions

    {:name => "#{@name}ClientStub.cpp",
     :text => @@templates[:client_stub_def].build(fields)}
  end

  def generate(output)
    source = Array.new
    source << to_wrapper_decl
    source << to_wrapper_def
    source << to_server_stub_decl
    source << to_client_stub_decl
    source << to_client_stub_def

    @actions.each {|action| source << action.to_action_param_list_decl(@name) }
    @actions.each {|action| source << action.to_action_param_list_def(@name) }

    source.each do |file|
      File.open(File.join(output, file[:name]), 'w') do |io|
        io.write(file[:text])
      end
    end
  end
end

service_text = nil
File.open('example.res', 'r') do |file|
  service_text = file.read
end

service = RpcResource.new
service.parse(service_text.match(RpcResource::RESOURCE_REGEX).to_s)
service.generate('output_src')

