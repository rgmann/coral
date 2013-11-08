require File.join(File.expand_path('./'), 'option_hash.rb')

class RpcResource

  RESOURCE = 'resource'
  REGEX = /\b#{RESOURCE}\s+[\w_]+\s*\{[=;\s\w\_\&\(\),]*\}\s*/m

  attr_accessor :name
  attr_accessor :actions

  @@templates = {:server_stub_decl => TextTemplate.new('templates/server_resource_stub_h.template'),
                 :inst_wrapper_decl => TextTemplate.new('templates/instance_wrapper_h.template'),
                 :inst_wrapper_def => TextTemplate.new('templates/instance_wrapper_cpp.template'),
                 :inst_wrapper_user_def => TextTemplate.new('templates/instance_wrapper_user_cpp.template'),
                 :client_stub_decl => TextTemplate.new('templates/client_stub_resource_h.template'),
                 :client_stub_def => TextTemplate.new('templates/client_stub_resource_cpp.template')}

  def initialize(resource_text)
    @actions = Array.new
    @options = OptionHash.new 

    @options.add(:allow_exceptions, false, [true, false], lambda { |value|
      cval = nil
      cval = false if ['false', 'no'].include? value
      cval = true if ['true', 'yes'].include? value
      cval
    })

    parse(resource_text)
  end

  def parse(definition)
    @name = definition.match(/\b#{RESOURCE}\s+\w+\s*\{/m).to_s.split(/[\s\{]/)[1].to_s
    actions = definition.to_s.match(/\{.*\}/m).to_s

    # Parse the body of the resource definition.
    actions = actions.scan(/\{([^\}]+)\}/m).last.first
    rules = actions.split(';')
    rules.each {|rule| rule.strip! }
    rules.each do |rule|
      next if rule.empty?
      if rule.match(/\b\w+\s+\b\w+\s*\(.*\)/)
        action = rule.match(/\b\w+\s+\b\w+\s*\(.*\)/).to_s
        param_list = action.scan(/\(([^\)]+)\)/).last
        if param_list
          param_list = param_list.first.split(',')
          param_list.each {|param| param.strip! }
        end
        type_name = action.match(/\b\w+\s+\b\w+\s*\(/)
        type, action_name = type_name.to_s.split(/[\s+\(]/)
        @actions << RpcResourceAction.new(@name, action_name, param_list, type)
      else
        parse_resource_property(rule)
      end
    end

    if @options[:allow_exceptions]
      @actions.each {|action| action.allow_exceptions = true }
    end
  end

  def parse_resource_property(rule)
    name_val_pair = /\b\w+\s*=\s*\b\w+/
    if not rule.match(name_val_pair).nil?
      tokens = rule.partition("=")
      name = tokens[0].strip
      value = tokens[2].strip

      @options[name] = value if @options.include?(name)
    else
      name = rule
      @options[name] = true if @options.include?(name)
    end
  end

  def to_wrapper_decl
    fields = Hash.new
    fields['COMP_GUARD'] = @name.upcase
    fields['WRAPPED_CLASS'] = @name
    
    delegates = Array.new
    wrappers = Array.new
    @actions.each do |action|
      delegates << action.name
      wrappers << action.to_wrapper_action_decl
    end
    fields['ACTIONS'] = delegates
    fields['ACTION_WRAPPERS'] = wrappers

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
      action_plists.concat(action.param_list_names)
    end
    fields['ACTION_STRUCTS'] = action_plists
    fields['ACTIONS'] = actions

    {:name => "#{@name}Wrapper.cpp",
     :text => @@templates[:inst_wrapper_def].build(fields)}
  end

  def to_wrapper_user_def
    fields = {}
    fields['WRAPPED_CLASS'] = @name

    actions = []
    @actions.each do |action|
      actions << action.to_wrapper_user_action_def(fields).join
    end

    fields['ACTIONS'] = actions

    {:name => "#{@name}Wrapper_user.cpp",
     :text => @@templates[:inst_wrapper_user_def].build(fields)}
  end

  def to_server_stub_decl
    fields = Hash.new
    fields['COMP_GUARD'] = @name.upcase
    fields['WRAPPED_CLASS'] = @name

    actions = Array.new
    @actions.each do |action|
      actions << "      addAction(\"#{action.name}\", #{@name}Wrapper::#{action.name}Delegate);"
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
    dependencies = Array.new
    @actions.each do |action|
      actions << action.to_client_stub_action
      action.in_params.unresolved_fields("#include \"%s.h\"").each do |dep|
        dependencies << dep unless dependencies.include? dep
      end
    end
    fields['ACTIONS'] = actions
    fields['INCLUDES'] = dependencies

    {:name => "#{@name}ClientStub.h",
     :text => @@templates[:client_stub_decl].build(fields)}
  end

  def to_client_stub_def
    fields = Hash.new
    fields['WRAPPED_CLASS'] = @name

    actions = Array.new
    action_plists = Array.new
    @actions.each do |action| 
      allow_exceptions = @options[:allow_exceptions]
      actions << action.to_client_stub_action_def(fields).join
      action_plists.concat(action.param_list_names)
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

    @actions.each do |action|
      includes.concat(action.to_action_param_list_decl)
      source.concat(action.to_action_param_list_def)
    end

    includes.each do |file|
      File.open(File.join(inc_path, file[:name]), 'w') do |io|
        io.write(file[:text].join)
      end
    end

    source.each do |file|
      File.open(File.join(src_path, file[:name]), 'w') do |io|
        io.write(file[:text].join)
      end
    end
  end

  def declarations
    includes = Array.new
    includes << to_wrapper_decl
    includes << to_server_stub_decl
    includes << to_client_stub_decl
    @actions.each {|action| includes.concat(action.to_action_param_list_decl) }
    includes
  end

  def definitions
    source = Array.new
    source << to_wrapper_def
    source << to_wrapper_user_def
    source << to_client_stub_def
    @actions.each {|action| source.concat(action.to_action_param_list_def) }
    source
  end
end
