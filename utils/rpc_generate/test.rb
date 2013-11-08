require File.join(File.expand_path('./'), 'option_hash.rb')

ohash = OptionHash.new
ohash.add('allow_exceptions',
          false,
          [true, false],
          lambda { |value|
  return false if ['false', 'no'].include? value
  return true if ['true', 'yes'].include? value
  return nil
})

puts "value = #{ohash[:allow_exceptions]}"
ohash[:allow_exceptions] = 'true'
puts "value = #{ohash[:allow_exceptions]}"
ohash[:allow_exceptions] = nil
puts "value = #{ohash[:allow_exceptions]}"

