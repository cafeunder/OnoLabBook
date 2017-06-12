module ReVIEW
	class Compiler
		defblock :listwide, 2..3
		defblock :emlistwide, 0..2
		defblock :listnumwide, 2..3
		defblock :emlistnumwide, 0..2
		defblock :cmdwide, 0..1
	end

	class Builder
		def listwide(lines, id, caption, lang = nil) list(lines, id, caption, lang) end
		def emlistwide(lines, caption = nil, lang = nil) emlist(lines, caption, lang) end
		def listnumwide(lines, id, caption, lang = nil) listnum(lines, id, caption, lang) end
		def emlistnumwide(lines, caption = nil, lang = nil) emlistnum(lines, caption, lang) end
		def cmdwide(lines, caption = nil, lang = nil) cmd(lines, caption) end
	end

	class LATEXBuilder
		def listwide(lines, id, caption, lang = nil)
			common_code_block(id, lines, 'reviewlist*', caption, lang) do |line, idx|
				detab(line) + "\n"
			end
		end

		def emlistwide(lines, caption = nil, lang = nil)
			blank
			common_code_block(nil, lines, 'reviewemlist*', caption, lang) do |line, idx|
				detab(line) + "\n"
			end
		end

		def listnumwide(lines, id, caption, lang = nil)
			first_line_num = get_line_num
			common_code_block(id, lines, 'reviewlist*', caption, lang) do |line, idx|
				detab((idx+first_line_num).to_s.rjust(2)+": " + line) + "\n"
			end
		end

		def emlistnumwide(lines, caption = nil, lang = nil)
			blank
			first_line_num = get_line_num
			common_code_block(nil, lines, 'reviewemlist*', caption, lang) do |line, idx|
				detab((idx+first_line_num).to_s.rjust(2)+": " + line) + "\n"
			end
		end

		def cmdwide(lines, caption = nil, lang = nil)
			blank
			common_code_block(nil, lines, 'reviewcmd*', caption, lang) do |line, idx|
				detab(line) + "\n"
			end
		end
	end
end
