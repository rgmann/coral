class TextTemplate

  attr_reader :lines

  def initialize(template_filename, marker = '$$')

    @lines = Array.new
    @markers = Hash.new
    @values = Hash.new
    @marker_regex = Regexp.new(Regexp.escape(marker))

    begin
      File.open(template_filename, 'r') do |file|
        scan_template(file)
      end 
    rescue
    end

    @values['tab'] = '   '
  end

  def scan_template(file)
    while not file.eof?
      line = file.readline
      @lines << line
      markers = line.scan(/#{@marker_regex}[\w_]+#{@marker_regex}/)
      markers.each do |marker|
        fields = marker.scan(/#{@marker_regex}([\w_]+)#{@marker_regex}/).first
        fields.each do |field|
          @markers[field] = marker
        end
      end
    end
  end

  def set(field, value)
    raise "Invalid type" unless value.class == String or value.class == Array
    @values[field] = value
  end

  def fields
    @markers.keys
  end

  def build(ext_values)
    values = @values.merge(ext_values)
    rlines = Array.new
    @lines.each do |line|
      rline = line.dup
      @markers.each do |field, marker|
        if line.match(/#{Regexp.escape(marker)}/) and values[field].nil?
          raise "Failed to find value match for template field '#{field}'." 
        end
        next unless values[field] and values[field].class == String
        rline.gsub!(marker, values[field])
      end
      @markers.each do |field, marker|
        next unless values[field] and values[field].class == Array
        next unless line.match(/#{Regexp.escape(marker)}/)
        rlinarr = Array.new
        values[field].each do |value|
          rlinarr << rline.dup.gsub(marker, value) if value
        end
        rline = rlinarr
      end
      rlines << rline if rline.class == String
      rline.each { |tline| rlines << tline } if rline.class == Array
    end

    rlines
  end

end


