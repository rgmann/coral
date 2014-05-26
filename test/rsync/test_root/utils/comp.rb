
byte_count = 0
diff_count = 0

def size_of(file)
  block_size = File.stat(file).blksize
  block_count = File.stat(file).blocks
  block_size * block_count
end

File.open(ARGV[0], 'r') do |left|
  File.open(ARGV[1], 'r') do |right|
    while not left.eof? and not right.eof? do
      left_byte = left.read(1)
      right_byte = right.read(1)

      if left_byte.nil?
        puts "left nil at #{byte_count}"
      elsif right_byte.nil?
        puts "right nil at #{byte_count}"
      elsif left_byte != right_byte
        left_byte = left_byte.unpack('C').first
        right_byte = right_byte.unpack('C').first
        puts "at #{byte_count} left=#{'%02X' % left_byte}, right=#{'%02X' % right_byte}"
        diff_count += 1
      else
        byte_count += 1
      end
    end
  end
end

puts "byte_count = #{byte_count}"
puts "size_of(left)=#{File.size?(ARGV[0])}, size_of(right)=#{File.size?(ARGV[1])}"
puts "Files are #{(diff_count > 0) ? "NOT " : "" }equal."
