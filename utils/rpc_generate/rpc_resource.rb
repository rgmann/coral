
class RpcResource

  RESOURCE = 'resource'
  REGEX = /\b#{RESOURCE}\s+[\w_]+\s*\{[\s\w\_\&\(\),]*\}\s*/m

  attr_accessor :name
  attr_accessor :actions

  @@templates = {:server_stub_decl => TextTemplate.new('templates/server_resource_stub_h.template'),
                 :inst_wrapper_decl => TextTemplate.new('templates/instance_wrapper_h.template'),
                 :inst_wrapper_def => TextTemplate.new('templates/instance_wrapper_cpp.template'),
                 :client_stub_decl => TextTemplate.new('templates/client_stub_resource_h.template'),
                 :client_stub_def => TextTemplate.new('templates/client_stub_resource_cpp.template')}

  def initialize(resource_text)
    @actions = Array.new
    parse(resource_text)
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
      type, action_name = type_name.to_s.split(/[\s+\(]/)
      @actions << RpcResourceAction.new(@name, action_name, param_list, type)
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
    action_plists = Array.new
    @actions.each do |action|
      actions << action.to_wrapper_action_def(fields).join
      action_plists << action.param_list_name
    end
    fields['ACTION_STRUCT'] = action_plists
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
    action_plists = Array.new
    @actions.each do |action| 
      actions << action.to_client_stub_action_def(fields).join
      action_plists << action.param_list_name
    end
    fields['ACTION_STRUCT'] = action_plists
    fields['ACTIONS'] = actions

    {:name => "#{@name}ClientStub.cpp",
     :text => @@templates[:client_stub_def].build(fields)}
  end

  def generate(inc_path, src_path)
    includes = Array.new
    source = Array.new

    includes << to_wrapper_decl
    source << to_wrapper_def
    includes << to_server_stub_decl
    includes << to_client_stub_decl
    source << to_client_stub_def

    @actions.each {|action| includes << action.to_action_param_list_decl(@name) }
    @actions.each {|action| source << action.to_action_param_list_def(@name) }

    includes.each do |file|
      File.open(File.join(inc_path, file[:name]), 'w') do |io|
        io.write(file[:text])
      end
    end

    source.each do |file|
      File.open(File.join(src_path, file[:name]), 'w') do |io|
        io.write(file[:text])
      end
    end
  end
end
