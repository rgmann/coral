
prefix = 'dfile_'
output_path = ARGV[0]
size_bytes = 100000

filename = "#{prefix}0.dat"
server_filename = File.join(output_path, filename)
File.open(server_filename, 'w+') do |io|
  size_bytes.times do
    io.write [rand(256)].pack('C')
  end
end

similarity_mode = :block
block_size = 256
fill_mode = :none

File.open(server_filename, 'r') do |is|
  File.open(client_filename, 'w+') do |os|
    loop do
      begin
        block = is.read(block_size)
      rescue
        break
      end

      os.write(block)

    end
  end
end

