
LICENSE_END_TOKEN = '// %% license-end-token %%'

license_path = ARGV[0]
update_root = ARGV[1]

license_text = ''

if license_path && File.exist?( license_path )
  begin
    File.open( license_path, 'r' ) do |io|
      license_text = io.read
    end

    # Add a line before and after.
    license_text = "\n#{license_text}\n\n"

    license_text_lines = []
    license_text.lines.each do |line|
      license_text_lines << "// #{line.strip}\n"
    end
    license_text = license_text_lines.join

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


def partition_file( filename )
  found_license_end_token = false
  post_token_lines = []
  pre_token_lines = []

  File.open( filename, 'r' ) do |io|
    io.read.lines.each do |line|
      if line.include? LICENSE_END_TOKEN
        found_license_end_token = true
      elsif found_license_end_token
        post_token_lines << line
      else
        pre_token_lines << line
      end
    end
  end

  [ found_license_end_token, pre_token_lines, post_token_lines ]
end

Dir.glob( File.join( File.expand_path( update_root ), '**', '*.{h,cpp}' ) ) do |filename|

  unless ( ( filename.include? 'CMakeFiles') || ( File.basename( filename ).include? 'pb.h' ) )
    
    found_license_end_token, pre_token_lines, post_token_lines = partition_file( filename )

    # If no token was found, insert it.
    unless found_license_end_token
      new_file_content = [ "#{LICENSE_END_TOKEN}\n\n\n" ].concat( pre_token_lines ).join
      File.open( filename, 'w+' ) do |io|
        io.write new_file_content
      end
    end

    found_license_end_token, pre_token_lines, post_token_lines = partition_file( filename )

    if found_license_end_token
      new_file_content = [ "#{license_text}\n", "#{LICENSE_END_TOKEN}\n" ].concat( post_token_lines ).join
      File.open( filename, 'w+' ) do |io|
        io.write new_file_content
      end
    else
      puts "Failed to insert license token into #{filename}"
      exit 1
    end

  end

end
