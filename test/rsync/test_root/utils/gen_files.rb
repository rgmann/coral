
prefix = 'dfile_'
output_path = ARGV[0]
size_bytes = 100000

File.open(File.join(output_path, "#{prefix}0.dat"), 'w+') do |io|
  size_bytes.times do
    io.write [rand(256)].pack('C')
  end
end

