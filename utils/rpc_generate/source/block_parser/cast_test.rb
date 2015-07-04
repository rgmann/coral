require 'cast'

source = File.read(ARGV[0])
ast = C.parse(source)
ast.entities.each do |declaration|
  declaration.declarator.each do |declarator|
    puts "#{declarator.name}: declarator.type"
  end
end
