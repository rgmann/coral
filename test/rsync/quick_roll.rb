
M = 2 ** 16

bytes = nil
File.open(ARGV[0], 'r') do |io|
  bytes = io.read
end
bytes = bytes.unpack('C*')

offset = ARGV[1].to_i
segment_size = ARGV[2].to_i

k = offset
l = offset + segment_size
l = bytes.length - 1 if l >= bytes.length

a = 1
bytes[k..l].each { |byte| a += byte }
a = a % M

b = 0
bytes[k..l].each_index { |i|
  puts "i=#{i}, (l - (k + i) + 1)=#{(l - (k + i) + 1)}, #{bytes[k + i]}"
  b += (l - (k + i) + 1) * bytes[k + i]
}
b = b % M

puts "a=#{'0x%04X' % a}, b=#{'0x%04X' % b}"

