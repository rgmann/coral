class Option

  attr_accessor :value

  def initialize(params = {})
    raise "Option missing name" unless params.keys.include? :name
    raise "Option missing default" unless params.keys.include? :default

    @name = params[:name]
    @default = params[:default]
    @value = @default
    @conversion = params[:conversion]
    @value_set = Array.new
    @value_set = params[:set] if params[:set]
  end

  def set(value)
    opt_value = @conversion.call(value) if @conversion
    opt_value = value unless @conversion

    if not @value_set.empty? and not @value_set.include? opt_value
      opt_value = @default
    end

    @value = opt_value
  end
end

class OptionHash

  def initialize
    @options = Hash.new
  end

  def add(name, default, set = nil, conversion = nil)
    opt_name = name
    opt_name = :"#{name}" if name.class == String
    @options[opt_name] = Option.new(:name => name,
                                :default => default,
                                :set => set,
                                :conversion => conversion)
  end

  def [](name)
    opt_name = name
    opt_name = :"#{name}" if name.class == String
    return @options[opt_name].value if @options[opt_name]
    return nil
  end

  def []=(name, value)
    opt_name = name
    opt_name = :"#{name}" if name.class == String
    @options[opt_name].set(value) if @options[opt_name]
  end

  def include?(name)
    opt_name = name
    opt_name = name.to_sym if name.class == String
    @options.keys.include?(opt_name)
  end
end

