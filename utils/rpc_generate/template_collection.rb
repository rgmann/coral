require 'openssl'
require File.join(File.dirname(__FILE__), "text_template")

class TemplateCollection

  def initialize
    @templates = Hash.new
    @common_fields = Hash.new
  end

  def load_templates
    @templates.each do |key, value|
      value[:template] = TextTemplate.new(value[:template_path])
    end
  end

  def load_template(name)
    @templates[name][:template] = TextTemplate.new(@templates[name][:template_path])
  end

  def add_template(name, template_path, type = nil, file_name_fmt = nil)
    unless File.exists?(template_path)
      raise "Cannot find #{template_path}"
    end

    reload_template = false
    reload_template = true if not @templates.keys.include? name

    md5_hash = nil
    File.open(template_path, 'r') do |tfile|
      md5_hash = OpenSSL::Digest.digest('md5', tfile.read)
    end

    # If we think the file has already been loaded, but the hash
    # does not match, we need to reload.
    if not reload_template and md5_hash != @templates[name][:hash]
      reload_template = true
    end

    if reload_template
      @templates[name] = {
        :type => type,
        :file_name => file_name_fmt,
        :template_path => template_path,
        :hash => md5_hash
      }
      load_template(name)
    end
  end

  def add_common_field(name, value)
    @common_fields[name] = value
  end

  def create_descriptor (name, fields, prefix)

    unless @templates[name]
      raise "Could not find template with descriptor name: #{name.to_s}."
    end
    unless fields.class.to_s == "Hash"
      raise "Fields should be specified as a Hash object."
    end
    unless @templates[name][:type]
      raise "Template type required to generate a file descriptor."
    end
    unless @templates[name][:file_name]
      raise "Output file name format string required to generate a file descriptor."
    end
    unless @templates[name][:template]
      raise "No template for descriptor: #{name.to_s}."
    end


    # Build the file descriptor.
    descriptor = Hash.new
    descriptor[:type] = @templates[name][:type]
    descriptor[:name] = sprintf(@templates[name][:file_name], prefix)
    descriptor[:text] = fill_template(name, fields)

    descriptor
  end

  def fill_template(name, fields)

    unless @templates[name]
      raise "Could not find template with descriptor name: #{name.to_s}."
    end
    unless fields.class.to_s == "Hash"
      raise "Fields should be specified as a Hash object."
    end
    unless @templates[name][:template]
      raise "No template for descriptor: #{name.to_s}."
    end

    # Merge the common fields and per-file fields.
    combined_fields = fields.dup
    combined_fields.merge!(@common_fields)

    return @templates[name][:template].build(combined_fields)
  end
end
