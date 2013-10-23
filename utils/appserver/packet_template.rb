class PacketTemplate

   attr_accessor :classname
   attr_accessor :hdr_template
   attr_accessor :imp_template
   attr_accessor :parent
   attr_reader   :filename
   attr_accessor :hdr_header
   attr_accessor :imp_header
   attr_accessor :header_path
   attr_accessor :implementation_path

   def initialize
      @hdr_template = String.new
      @imp_template = String.new
      @classname = String.new
      @parent = "GenericPacket"
      @hdr_header = String.new
      @imp_header = String.new
      @header_path = ""
      @implementation_path = ""
   end

   def create

      false if @classname.empty?

      if not valid_class_name(@classname)
         puts "Invalid class name.  Please follow naming conventions."
         return false
      end

      success = (create_header() and create_implementation())

      if success
         hdr_path = @classname
         hdr_path = File.join(@header_path, @classname) unless @header_path.empty?
         File.open("#{hdr_path}.h", 'w+') do |io|
            io.write @hdr_template
         end

         imp_path = @classname
         imp_path = File.join(@implementation_path, @classname) unless @implementation_path.empty?
         File.open("#{imp_path}.cpp", 'w+') do |io|
            io.write @imp_template
         end
      end

      true
   end

   def create_header()

      defstr = class_name_to_def(@classname)

      @hdr_template << "#{@hdr_header}\n" unless @hdr_header.empty?
      @hdr_template << "#ifndef #{defstr}\n" 
      @hdr_template << "#define #{defstr}\n" 
      @hdr_template << "\n"
      @hdr_template << "#include \"#{@parent}.h\"\n" 
      @hdr_template << "\n"
      @hdr_template << "class #{@classname} : public #{@parent}\n"
      @hdr_template << "{\n" 
      @hdr_template << "public:\n"
      @hdr_template << "\n"
      @hdr_template << "   enum Type\n"
      @hdr_template << "   {\n"
      @hdr_template << "      // TODO: add types\n"
      @hdr_template << "   }\n"
      @hdr_template << "\n"
      @hdr_template << "   struct __attribute ((__packed__)) Data\n"
      @hdr_template << "   {\n"
      @hdr_template << "   };\n"
      @hdr_template << "\n"
      @hdr_template << "   explicit #{@classname}();\n"
      @hdr_template << "\n"
      @hdr_template << "   bool  unpack(void* pPkt, ui32 nSizeBytes);\n"
      @hdr_template << "\n"
      @hdr_template << "private:\n"
      @hdr_template << "\n"
      @hdr_template << "   typedef #{@parent} inherited;\n"
      @hdr_template << "\n"
      @hdr_template << "};\n" 
      @hdr_template << "\n"
      @hdr_template << "#endif // #{defstr}\n" 

      true
   end

   def create_implementation
      function_separator = String.new
      function_separator << "//"
      function_separator << "-" * 78
      function_separator << "\n"

      @imp_template << "#{@imp_header}\n" unless @imp_header.empty?
      @imp_template << "#include <stdio.h>\n"
      @imp_template << "#include <string.h>\n"
      @imp_template << "#include \"#{@classname}.h\"\n"
      @imp_template << "\n"
      @imp_template << function_separator
      @imp_template << "#{@classname}::#{@classname}()\n"
      @imp_template << ": #{@parent}()\n"
      @imp_template << "{\n"
      @imp_template << "}\n"
      @imp_template << "\n"
      @imp_template << function_separator
      @imp_template << "bool #{@classname}::unpack(void* pPkt, ui32 nSizeBytes)\n"
      @imp_template << "{\n"
      @imp_template << "}\n"
      @imp_template << "\n"

      true
   end

   def valid_class_name(name)
      valid = (class_name_to_def(name).nil? == false)
      valid
   end

   def class_name_to_def(name)
      nil if name.nil? or name.empty? or name.index(/[A-Z]/) != 0

      defname = String.new

      toPos = 0
      fromPos = 0
      while not (fromPos = name.index(/[A-Z]/, toPos)).nil? do
         toPos = name.length if (toPos = name.index(/[A-Z]/, fromPos+1)).nil?
         defname << "#{name[fromPos..toPos-1].upcase}_"
         #defname << "_" unless toPos == name.length
         toPos = fromPos + 1
      end

      defname << "H"

      defname
   end
end
