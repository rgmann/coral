class TemplateMarker
  attr_accessor :marker
  attr_accessor :field_name
  attr_accessor :marker_text

  def initialize(marker_text)
    [@fieldName, sep, params] = marker_text.partition(':') 
    params.split(',').each do |param|
      [param_name, sep, value] = param.partition('=')
      if not param_name.empty? and not value.empty?
        @params[param_name] = value.to_i
      end
    end
  end

  def <=> (other)
    0 if other
  end

  def match_values(values)
    @value = values

  def self.scan(line, delimeter, values) 
    @marker_regex = Regexp.new(Regexp.escape(delimeter))
    markers = line.scan(/#{@marker_regex}[\w_]+#{@marker_regex}/).each
    markers.each do |marker|
      marker_text = marker.scan(/#{@marker_regex}([\w_]+)#{@marker_regex}/).first
      tmarkers << TemplateMarker.new(marker_text)
    end

    # Match the markers to values.
    tmarkers.each do |marker|
      marker.match_values(values)
    end

    tmarkers.sort!
    tmarkers
  end
end

class TextTemplate

  def self.process_line(line, markers, values)
    out_lines = Array.new
    markers.each do |marker|
      value = values[marker.field_name]
      raise "Missing value for field '#{marker.field_name}'" if value.nil?
        
      if value.class == Array
        
      else
      end
    end
  end

  def self.build(file, ext_values)
    values = @values.merge(ext_values)

    in_lines = Array.new
    begin
      File.open(file, 'r') { |io| in_lines = io.read.lines }
    rescue Exception => e
      raise "#{e.message}"
    end

    in_lines.each do |in_line|
      markers = TemplateMarker.scan(in_line, delimeter, values)
      array_field_count = 0
      out_lines.concat(process_line(in_line, markers, values))
    end

    out_lines
  end

end


