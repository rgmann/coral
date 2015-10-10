
LICENSE_EXCLUDE_TOKEN  = '// %% license-exclude-token %%'
LICENSE_END_TOKEN      = '// %% license-end-token %%'

command_name = ARGV[0]
license_path = ARGV[1]
update_root  = ARGV[2]

text_to_insert = ''


unless ['token', 'before', 'after'].include? command_name
  puts "Invalid command."
  exit 1
end

if license_path && File.exist?( license_path )
  begin
    File.open( license_path, 'r' ) do |io|
      text_to_insert = io.read
    end

    # Add a line before and after.
    text_to_insert = "\n#{text_to_insert}\n"

    license_text_lines = []
    text_to_insert.lines.each do |line|
      license_text_lines << "// #{line.strip}\n"
    end
    text_to_insert = license_text_lines.join

  rescue
    puts "Failed to read license text from #{license_path}"
    exit 1
  end
else
  puts "Specify path to license file."
  exit 1
end

unless update_root
  puts "Specify path to directory where license should be applied."
  exit 1
end


class PartitionRecord
  attr_reader :found_license_end_token
  attr_reader :found_license_exclusion

  attr_reader :pre_token_lines
  attr_reader :post_token_lines

  def initialize( filename )
    @found_license_exclusion = false
    @found_license_end_token = false
    @pre_token_lines = []
    @post_token_lines = []

    File.open( filename, 'r' ) do |io|
      io.read.lines.each do |line|
        if line.include? LICENSE_END_TOKEN
          @found_license_end_token = true

        elsif line.include? LICENSE_EXCLUDE_TOKEN
          @found_license_exclusion = true
          break

        elsif @found_license_end_token
          @post_token_lines << line

        else
          @pre_token_lines << line

        end
      end
    end
  end

end


Dir.glob( File.join( File.expand_path( update_root ), '**', '*.{h,cpp}' ) ) do |filename|

  unless ( ( filename.include? 'CMakeFiles') || ( File.basename( filename ).include? 'pb.h' ) )
    
    partition_record = PartitionRecord.new( filename )
    next if partition_record.found_license_exclusion

    # If no token was found, insert it.
    unless partition_record.found_license_end_token

      new_file_content = [ "#{LICENSE_END_TOKEN}\n\n\n" ].concat( partition_record.pre_token_lines ).join
      File.open( filename, 'w+' ) do |io|
        io.write new_file_content
      end
    end


    if ['before', 'after'].include? command_name
      partition_record = PartitionRecord.new( filename )

      if partition_record.found_license_end_token

        new_file_content = []
        if command_name == 'before'
          new_file_content.concat([
            "#{text_to_insert}\n",
            "#{LICENSE_END_TOKEN}\n"
          ]).concat( partition_record.post_token_lines )

        else
          new_file_content.concat( partition_record.pre_token_lines ).concat([
            "#{LICENSE_END_TOKEN}\n",
            "#{text_to_insert}\n"
          ]).concat( partition_record.post_token_lines )

        end

        new_file_content = new_file_content.join

        File.open( filename, 'w+' ) do |io|
          io.write new_file_content
        end
      else
        puts "Failed to insert license token into #{filename}"
        exit 1
      end
    end

  end

end
