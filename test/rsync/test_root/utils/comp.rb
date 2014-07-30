
verbose = false

ARGV.each do |arg|
  if arg.downcase == '-v'
    verbose = true
  end
end

left_file = ARGV[-2]
right_file = ARGV[-1]

unless left_file and File.exist?(left_file)
  puts "Specify valid left file path"
  exit
end

unless right_file and File.exist?(right_file)
  puts "Specify valid right file path"
  exit
end


byte_count = 0
diff_count = 0

File.open(left_file, 'r') do |left|
  File.open(right_file, 'r') do |right|
    while not left.eof? and not right.eof? do
      left_byte = left.read(1)
      right_byte = right.read(1)

      if left_byte.nil?
        puts "left nil at #{byte_count}" if verbose
      elsif right_byte.nil?
        puts "right nil at #{byte_count}" if verbose
      elsif left_byte != right_byte
        left_byte = left_byte.unpack('C').first
        right_byte = right_byte.unpack('C').first
        puts "at #{byte_count} left=#{'%02X' % left_byte}, right=#{'%02X' % right_byte}" if verbose
        diff_count += 1
      else
      end
      byte_count += 1
    end
  end
end

puts "byte_count = #{byte_count}"
puts "size_of(left)=#{File.size?(ARGV[0])}, size_of(right)=#{File.size?(ARGV[1])}"
puts "Files are #{(diff_count > 0) ? "NOT " : "" }equal."
