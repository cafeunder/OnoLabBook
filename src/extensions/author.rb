module ReVIEW
	class Compiler
		definline :author
	end

	class Builder
		def inline_author(str) end
	end

	class LATEXBuilder
		def inline_author(str)
			macro('setauthorname', escape(str))
		end
	end
end
