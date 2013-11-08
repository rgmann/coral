
class Field

  TYPES = ['i32',
           'i64',
           'bool',
           'double',
           'string',
           'structure']

  TYPE_MAP = {'i32' => {:decl => 'i32', :default => '0'},
              'i64' => {:decl => 'i64', :default => '0'},
              'bool' => {:decl => 'bool', :default => 'false'},
              'double' => {:decl => 'double', :default => '0.0'},
              'string' => {:decl => 'std::string', :default => "\"\""}}

  REF_TYPE = {:value => '%s %s',
              :ref => '%s& %s',
              :cref => 'const %s& %s'}

  attr_accessor :type
  attr_accessor :index
  attr_accessor :name
  attr_accessor :default
  attr_accessor :ref_type

  def initialize(params = {})
    @type = params[:type]
    @name = params[:name]
    @ref_type = params[:ref_type]
  end

  def parse(param_text)

    unless /(?:c?ref\s)?\b[a-zA-Z0-9_]+\s+\b[a-zA-Z0-9_]+\s*[,\n]?/.match(param_text)
      raise "Error in field definitions."
    end

    tokens = param_text.split(/\s+/)
    if tokens.count.eql? 2
      @ref_type = :value
      @type = tokens[0].match(/\b\w+/).to_s
      @name = tokens[1].match(/\b\w+/).to_s
    else
      @ref_type = tokens[0].match(/\b\w+/).to_s.to_sym
      if not REF_TYPE.keys.include? @ref_type
        raise "Unknown type modifier \"#{@ref_type.to_s}\""
      end

      @type = tokens[1].match(/\b\w+/).to_s
      @name = tokens[2].match(/\b\w+/).to_s
    end
  end

  def valid_type
    TYPES.include?(@type)
  end

  def valid?
    valid = (@type.nil? == false)
    valid &= (@name.nil? == false)
    valid &= (@ref_type.nil? == false)
    valid
  end

  def is_ref
    true if @ref_type == :ref
    false
  end

  def is_cref
    true if @ref_type == :cref
    false
  end

  def declaration(params = {})
    decl = ''

    decl_type = @type
    decl_type = TYPE_MAP[@type][:decl] if TYPE_MAP.keys.include? @type

    decl = REF_TYPE[@ref_type] % [decl_type, @name] if params[:type].nil?
    case params[:type]
    when :value
      decl = REF_TYPE[:value] % [decl_type, @name]
    when :ref
      decl = REF_TYPE[:ref] % [decl_type, @name]
    when :cref
      decl = REF_TYPE[:cref] % [decl_type, @name]
    else
    end

    if params[:init] and TYPE_MAP.keys.include? @type
      decl = "#{decl} = #{TYPE_MAP[@type][:default]}"
    end

    decl
  end
end

