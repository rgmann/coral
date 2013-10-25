
class Field

  TYPES = ['i32',
           'i64',
           'bool',
           'double',
           'string',
           'structure']

  VALUE = ''
  REFERENCE = '&'

  attr_accessor :type
  attr_accessor :index
  attr_accessor :name
  attr_accessor :default
  attr_accessor :pbtype

  def initialize(params = {})
    @type = params[:type]
    @name = params[:name]
    @pbtype = params[:ref_type]

    declaration = params[:declaration]
    if declaration
      raise "Error in field definition: #{declaration}" unless declaration.match(/\b\w+\s+\b\w+\s*[,\n]/)
      @type, @name = declaration.scan(/\b\w+/)
    end
  end

  def self.valid_type(type)
    TYPES.include?(type)
  end

  def to_decl(param_type = VALUE)
    "#{@type}#{param_type} #{@name}"
  end
end

