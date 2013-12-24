
class Field

  TYPES = ['bool',
           'double',
           'string',
           'i32', 'int32', 'uint32', 'sint32', 'fixed32', 'sfixed32',
           'i64', 'int64', 'uint64', 'sint64', 'fixed64', 'sfixed64']

  TYPE_MAP = {'i32' => {:decl => 'i32', :default => '0'},
              'i64' => {:decl => 'i64', :default => '0'},
              'bool' => {:decl => 'bool', :default => 'false'},
              'double' => {:decl => 'double', :default => '0.0'},
              'string' => {:decl => 'std::string', :default => "\"\""}}

  REF_TYPE = {:value => '%s %s',
              :ref => '%s& %s',
              :cref => 'const %s& %s'}

  TO_GPB_TYPE = { 'i32' => 'sint32',
                  'u32' => 'uint32',
                  'i64' => 'sint64',
                  'u64' => 'uint64'}

  attr_accessor :type
  attr_accessor :index
  attr_accessor :name
  attr_accessor :default
  attr_accessor :ref_type
  attr_accessor :namespace

  def initialize(params = {})
    @type = params[:type]
    @name = params[:name]
    @ref_type = params[:ref_type]
    @namespace = nil
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

  def base_scalar?
    TYPES.include?(@type)
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

  def proto(ref_type = nil)
    proto = ''
    proto_type = @type
    proto_type = TYPE_MAP[@type][:decl] if TYPE_MAP.keys.include? @type
    proto_type = "#{@namespace}::#{proto_type}" if @namespace
    proto = REF_TYPE[@ref_type] % [proto_type, ''] if ref_type.nil?

    if ref_type
      if [:value, :ref, :cref].include?(ref_type)
        proto = REF_TYPE[ref_type] % [proto_type, '']
      else
        raise "Invalid reference type: #{ref_type.to_s}"
      end
    end

    proto
  end

  def declaration(params = {})
    ref_type = @ref_type
    ref_type = params[:type] if params[:type]

    decl = "#{proto(ref_type)}#{@name}"
    if params[:init] and TYPE_MAP.keys.include? @type
      decl = "#{decl} = #{TYPE_MAP[@type][:default]}"
    end

    decl
  end

  def gpb_type
    type = TO_GPB_TYPE[@type]
    type = @type unless type
    type
  end

  def gpb_setter
    return "set_#{@name}(#{@name})"
  end

  def gpb_getter
    return "#{@name}()"
  end
end

