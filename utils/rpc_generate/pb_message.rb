class PbMessage

  attr_accessor :type
  attr_accessor :namespace
  attr_accessor :body
  attr_accessor :fields

  def initialize(type, namespace = nil, body = nil)
    tokens = type.split('.')
    if tokens.count > 1
      @type = tokens.last
      @namespace = tokens.take(tokens.count - 1).join('.')
    else
      @type = type
      @namespace = namespace
    end

    @body = body
  end

  def declaration (params = {})
    decl_string = ""
    decl_string << "const " if [:const_reference].include?(params[:format])
    decl_string << "#{@namespace}::" if @namespace and params[:nspace] == true
    decl_string << @type
    decl_string << "&" if [:const_reference, :reference].include?(params[:format])
    decl_string << " #{params[:name]}" if params[:name]
    decl_string
  end

  def to_message_decl
    decl = nil
    if @body
      decl = "message #{@type} {\n"
      decl << body
      decl << "}\n"
    end
    decl
  end
end

